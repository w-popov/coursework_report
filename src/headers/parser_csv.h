#ifndef _PARSER_CSV_H_
#define _PARSER_CSV_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

// Цвета
#define RED "\033[31m"
#define RED_BOLD "\033[1;31m"
#define GREEN "\033[32m"
#define GREEN_BOLD "\033[1;32m"
#define YELLOW "\033[33m"
#define BLUE "\033[34m"
#define MAGENTA "\033[35m"
#define CYAN "\033[36m"
#define WHITE "\033[37m"
#define RESET "\033[0m"

#define INITIAL_CAPACITY_SVECTOR 1024 // Начальная ёмкость SVector
#define REALLOC_CAPACITY_SVECTOR 4 // Во сколько раз увеличится размер вектора
#define MAX_FIELD_SIZE 64 // Максимальный размер поля в CSV-файле
#define LEN_ERR_MSG 255 // Максимальная длина сообщения об ошибке

struct IStorage_t;

// Указатели на интерфейсные ф-ции
typedef void *(*StoragePush)(struct IStorage_t *self, void *item);
typedef void *(*StorageGet)(struct IStorage_t *self, size_t index);
typedef void (*StorageFree)(struct IStorage_t *self);
typedef size_t (*StorageSize)(struct IStorage_t *self);
typedef void *(*StorageData)(struct IStorage_t *self);

/**
 * @brief Интерфейс хранилища
 */
struct IStorage_t
{
    StoragePush push; // Добавить элемент
    StorageGet get;   // Получить элемент по индексу
    StorageFree free; // Полностью освободить память
    StorageSize size; // Кол-во элементов в хранилище
    StorageData raw_data; // Указатель массив данных
};

/**
 * Динамический массив.
 */
struct SVector
{
    struct IStorage_t storage; // Источник хранения
    void *data;                // Указатель на массив
    size_t capacity;  // Ёмкость (кол-во элементов)
    size_t size;      // Текущее кол-во элементов
    size_t size_item; // Размер одного элемента sizeof(item)
};

/**
 * Коды ошибок парсинга
 */
typedef enum {
    ERR_VALIDATE = 8888,
    ERR_VALIDATE_MAX_FIELD_SIZE,
    ERR_STRUCT_EXTRA_FIELD,
    ERR_STRUCT_LESS_FIELDS,
    ERR_STRUCT_MORE_FIELDS,
    ERR_STRUCT_FINAL_STR_LESS_FIELD,
    ERR_FINAL_MORE_FIELD,
    ERR_FIND_LESS_FIELD,
    ERR_FIND_MORE_FIELD
} ErrorInfo;

/**
 * Ошибки
 */
struct ErrorParse
{
    struct IStorage_t storage;       // Источник хранения
    char error_message[LEN_ERR_MSG]; // Сообщение об ошибке
    size_t error_row;                // Номер строки ошибки
    int16_t error_column;            // Номер колонки ошибки
};

/**
 * @brief Указатель на callback-функцию для посимвольного чтения
 */
typedef int (*GetNextCharCallback)(void *source);

/**
 * @brief Указатель на callback-функцию для прогрессбара
 */
typedef int64_t (*GetPosCallback)(void *source);

struct ParseSource
{
    void *stream; // Указатель на FILE* или на строку char*
    GetNextCharCallback get_char; // Функция чтения
    GetPosCallback
        get_pos; // Функция получения текущей позиции (для прогресс-бара)
};

struct ContextParser;

/**
 * @brief Указатель на callback-функцию индикатора выполнения
 */
typedef void (*CallbackProgressBar)(int64_t current, int64_t total);

/**
 * @brief Указатель на callback-функцию записи в массив данных
 */
typedef int (*CallbackWriteToArray)(struct ContextParser *ctx);

/**
 * Контекст для работы c csv
 */
struct Csv
{
    char buffer[MAX_FIELD_SIZE]; // Буфер для хранения текущей ячейки
    const char *delimiter;  // Разделитель
    size_t current_row;     // Индекс строки
    int16_t current_column; // Текущая колонка
    uint16_t length_field;  // Размер ячейки (столбца)
    uint16_t nums_field; // Количество столбцов (счет с нуля)
};

/**
 * Обратные вызовы
 */
struct Callbacks
{
    CallbackProgressBar clb_progress; // Указатель на ф-цию прогрессбара
    CallbackWriteToArray
        clb_write_to_arr; // Указатель на ф-цию записи в массив данных
};

/**
 * Контекст парсера
 */
struct ContextParser
{
    struct Csv csv; // Контекст для работы парсера
    struct Callbacks clbs; // Обратные вызовы
    struct IStorage_t *array; // Указатель на массив структук данных
    struct IStorage_t *errors_parse; // Указатель на массив структур ошибок
    size_t file_size; // Размер файла
};

/**
 * @brief Посимвольное чтение из файла
 */
int get_char_from_file (void *stream);

/**
 * @brief Посивольное чтение из строки
 */
int get_char_from_string (void *stream);

/**
 * @brief Позиция в файле для прогрессбара
 */
int64_t get_pos_from_file (void *stream);

/**
 * @brief Инициализация.
 * @param *storage хранилище
 * @param size_item размер элемента вектора
 * @param cap задать ёмкость, если передано 0, то ёмкость по умолчанию
 * @return указатель на хранилище, или NULL
 */
struct IStorage_t *svector_init (struct IStorage_t *storage, size_t size_item,
                                 size_t cap);

/**
 * @brief Добавить один элемент.
 * @param *storage указатель на хранилище
 * @param *item указатель на добавляемый элемент
 * @return *void указатель на массив data, или NULL
 */
void *svector_push (struct IStorage_t *storage, void *item);

/**
 * @brief Получить элемент по индексу.
 * @param *storage указатель на хранилище
 * @param index индекс
 * @return *void указатель на элемент, или NULL
 */
void *svector_get (struct IStorage_t *storage, size_t index);

/**
 * @brief Вернуть массив данных
 * @param *storage указатель на хранилище
 */
void *svector_data (struct IStorage_t *storage);

/**
 * @brief Освободить память
 * @param *storage указатель на хранилище
 * @return void
 */
void svector_free (struct IStorage_t *storage);

/**
 * @brief Количество элементов в массиве
 * @param *storage указатель на хранилище
 * @return Количество элементов в массиве
 */
size_t svector_size (struct IStorage_t *storage);

/**
 * @brief Открыть файл
 * @param *filename имя файла
 * @param *fsize передача по указателю размера файла
 * @return Дескриптор открытого файла
 */
FILE *open_file (const char *filename, int64_t *fsize);

/**
 * @brief Запуск парсинга .csv файла
 * @param *ctx Указатель на контекст
 * @param *source источник данных
 * @return Указатель на контекст или NULL
 */
struct ContextParser *parse_csv (struct ContextParser *ctx,
                                 struct ParseSource *source);

/**
 * Добавить текст ошибки в массив
 */
void push_error (struct ContextParser *context, ErrorInfo err);

/**
 * @brief Вывод ошибок
 * @param *storage указатель на хранилище ошибок
 * @param rows кол-во строк для информативности
 */
void show_errors (struct IStorage_t *storage, size_t rows);

#ifdef __cplusplus
}
#endif

#endif