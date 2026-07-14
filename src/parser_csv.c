#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include "parser_csv.h"

/**
 * Добавить текст ошибки в массив
 */
void push_error(struct ContextParser *context, ErrorInfo err)
{
    struct ErrorParse error = {0};
    switch (err)
    {
    case ERR_VALIDATE:
        snprintf(
            error.error_message, 
            LEN_ERR_MSG, 
            "Ошибка валидации: Строка %zu, Колонка %d. Неверный формат данных: \"%s\"\n", 
            context->csv.current_row + 1, 
            context->csv.current_column + 1,
            context->csv.buffer
        );
        break;

    case ERR_VALIDATE_MAX_FIELD_SIZE:
        snprintf(
            error.error_message, 
            LEN_ERR_MSG, 
            "Ошибка валидации: Строка %zu, Колонка %d. Размер поля превышает %d\n", 
            context->csv.current_row + 1, 
            context->csv.current_column + 1,
            MAX_FIELD_SIZE
        );
        break;

    case ERR_STRUCT_EXTRA_FIELD:
        snprintf(
            error.error_message, 
            LEN_ERR_MSG, 
            "Ошибка структуры: Строка %zu содержит избыточные поля (обнаружена лишняя колонка %d)\n", 
            context->csv.current_row + 1, 
            context->csv.current_column + 1
        );
        break;

    case ERR_STRUCT_LESS_FIELDS:
        snprintf(
            error.error_message, 
            LEN_ERR_MSG, 
            "Ошибка структуры: Строка %zu содержит меньше полей (%d из %d)\n", 
            context->csv.current_row + 1, 
            context->csv.current_column + 1,
            context->csv.nums_field + 1
        );
        break;

    case ERR_STRUCT_MORE_FIELDS:
        snprintf(
            error.error_message, 
            LEN_ERR_MSG, 
            "Ошибка структуры: Строка %zu содержит больше полей (%d из %d)\n", 
            context->csv.current_row + 1, 
            context->csv.current_column + 1,
            context->csv.nums_field + 1
        );
        break;

    case ERR_STRUCT_FINAL_STR_LESS_FIELD:
        snprintf(
            error.error_message, 
            LEN_ERR_MSG, 
            "Ошибка структуры: Финальная строка %zu содержит меньше полей (%d из %d)\n", 
            context->csv.current_row + 1, 
            context->csv.current_column + 1,
            context->csv.nums_field + 1
        );
        break;

    case ERR_FINAL_MORE_FIELD:
        snprintf(
            error.error_message, 
            LEN_ERR_MSG, 
            "Ошибка: Финальная строка %zu содержит больше полей (%d из %d)\n", 
            context->csv.current_row + 1, 
            context->csv.current_column + 1,
            context->csv.nums_field + 1
        );
        break;

    case ERR_FIND_LESS_FIELD:
        snprintf(
            error.error_message, 
            LEN_ERR_MSG, 
            "Ошибка: Строка %zu оборвалась, обнаружено меньше полей (%d из %d)\n", 
            context->csv.current_row + 1, 
            context->csv.current_column,
            context->csv.nums_field + 1
        );
        break;

    case ERR_FIND_MORE_FIELD:
        snprintf(
            error.error_message, 
            LEN_ERR_MSG, 
            "Ошибка: Строка %zu содержит больше полей (%d из %d)\n", 
            context->csv.current_row + 1, 
            context->csv.current_column + 1,
            context->csv.nums_field + 1
        );
        break;
    
    default:
        snprintf(
            error.error_message, LEN_ERR_MSG, 
            "Неизвестная ошибка: Строка %zu, Колонка %d\n", 
            context->csv.current_row + 1, 
            context->csv.current_column + 1
        );
        break;
    }
    error.error_column = context->csv.current_column + 1;
    error.error_row = context->csv.current_row + 1;
    //svector_push(context->errors_parse, &error);
    context->errors_parse->push(context->errors_parse, &error);
}



/**
 * @brief Инициализация.
 * @param *storage хранилище
 * @param size_item размер элемента вектора
 * @param cap задать ёмкость, если передано 0, то ёмкость по умолчанию
 * @return указатель на хранилище, или NULL 
 */
struct IStorage* svector_init(struct IStorage *storage, size_t size_item, size_t cap)
{
    if (!storage || !size_item)
    {
        return NULL;
    }
    // Назначение интерфейсных функций для вектора
    storage->push = svector_push;
    storage->get  = svector_get;
    storage->free = svector_free;
    storage->size = svector_size;
    storage->raw_data = svector_data;
    
    // Приведение типа
    struct SVector *vec = (struct SVector*)storage;
    vec->size = 0;
    vec->capacity = (cap > 0) ? cap : INITIAL_CAPACITY_SVECTOR;
    vec->size_item = size_item;
    
    vec->data = malloc(vec->capacity * vec->size_item);
    if (vec->data == NULL)
    {
        vec->capacity = 0;
        return NULL;
    }
    
    return storage;
}


/**
 * @brief Добавить один элемент.
 * @param *storage указатель на хранилище
 * @param *item указатель на добавляемый элемент
 * @return *void указатель на массив data, или NULL
 */
void* svector_push (struct IStorage *storage, void *item)
{
    if (!storage || !item)
    {
        return NULL;
    }

    struct SVector *vec = (struct SVector*)storage;

    if (vec->size >= vec->capacity)
    {
        size_t new_capacity = vec->capacity * REALLOC_CAPACITY_SVECTOR;
        void *new_data = realloc(vec->data, new_capacity * vec->size_item);
        if (new_data == NULL)
        {
            return NULL;
        }
        vec->data = new_data;
        vec->capacity = new_capacity;
    }
    char *target = (char*)vec->data + (vec->size * vec->size_item);
    memcpy(target, item, vec->size_item);
    vec->size++;

    return vec->data;
}


/**
 * @brief Получить элемент по индексу.
 * @param *storage указатель на хранилище
 * @param index индекс
 * @return *void указатель на элемент, или NULL
 */
void* svector_get (struct IStorage *storage, size_t index)
{
    if (!storage)
    {
        return NULL;
    }

    struct SVector *vec = (struct SVector*)storage;

    if (index >= vec->size)
    {
        return NULL;
    }

    return (char*)vec->data + (index * vec->size_item);
}

/**
 * @brief Вернуть массив данных
 * @param *storage указатель на хранилище
 */
void* svector_data (struct IStorage *storage)
{
    if (!storage)
    {
        return NULL;
    } 
    return ((struct SVector*)storage)->data;
}


/**
 * @brief Освободить память
 * @param *storage указатель на хранилище
 * @return void
 */
void svector_free (struct IStorage *storage)
{
    if (!storage)
    {
        return;
    }

    struct SVector *vec = (struct SVector*)storage;

    if (vec->data)
    {
        free(vec->data);
        vec->data = NULL;
    }
    
    vec->size = 0;
    vec->capacity = 0;
    vec->size_item = 0;
}


/**
 * @brief Количество элементов в массиве
 * @param *storage указатель на хранилище
 * @return Количество элементов в массиве
 */
size_t svector_size (struct IStorage *storage)
{
    if (!storage) return 0;
    return ((struct SVector*)storage)->size;
}


/**
 * @brief Посимвольное чтение из файла
 */
int get_char_from_file(void *stream)
{
    return fgetc((FILE *)stream);
}


/**
 * @brief Позиция в файле для прогрессбара
 */
int64_t get_pos_from_file(void *stream)
{
    return (int64_t)ftell((FILE *)stream);
}


/**
 * @brief Посивольное чтение из строки
 */
int get_char_from_string(void *stream)
{
    char **str_ptr = (char **)stream;
    if (str_ptr == NULL || *str_ptr == NULL)
    {
        return EOF;
    }
    if (**str_ptr == '\0')
    {
        return EOF;
    } 
    
    return (unsigned char)(*(*str_ptr)++); 
}


/**
 * @brief Открыть файл
 * @param *filename имя файла
 * @param *fsize передача по указателю размера файла
 * @return Дескриптор открытого файла
 */
FILE *open_file (const char *filename, int64_t *fsize)
{
    FILE *file = fopen(filename, "r");
    if (file == NULL || fsize == NULL)
    {
        return NULL;
    }
    
    // Размер файла
    if (fseek(file, 0, SEEK_END) != 0) 
    {
        fclose(file);
        return NULL;
    }
    long file_size = ftell(file);
    if (fseek(file, 0, SEEK_SET) != 0) 
    {
        fclose(file);
        return NULL;
    }
    
    // буфер для оптимизации ввода-вывода
    setvbuf(file, NULL, _IOFBF, 65536);
    *fsize = file_size;
    return file;
}

/**
 * private функция для обработки разделителя
 */
static void except_delimeter (struct ContextParser *context)
{
    context->csv.buffer[context->csv.length_field] = '\0';
    context->csv.length_field = 0;
    context->csv.current_column++;
}

/**
 * private функция для обработки перевода строки
 */
static void except_newline (struct ContextParser *context)
{
    context->csv.buffer[context->csv.length_field] = '\0';
    context->csv.length_field = 0;
    context->csv.current_row++;
    context->csv.current_column = 0;
    memset(context->csv.buffer, 0, MAX_FIELD_SIZE);
}


/**
 * @brief Запуск парсинга .csv файла
 * @param *ctx Указатель на контекст
 * @param *source источник данных
 * @return Указатель на контекст или NULL
 */
struct ContextParser *parse_csv (struct ContextParser *context, struct ParseSource *source)
{
    if (!context || !source)
    {
        return NULL;
    }

    int character;
    int is_field_overflow = 0; 

    while ((character = source->get_char(source->stream)) != EOF)
    {
        if (character == '\r')
        {
            continue;
        }
        if (!is_field_overflow && context->csv.length_field >= MAX_FIELD_SIZE - 1)
        {
            if (character != *context->csv.delimiter && character != '\n')
            {
                is_field_overflow = 1;
                push_error(context, ERR_VALIDATE_MAX_FIELD_SIZE);
            }
        }
        if (character == *context->csv.delimiter)
        {
            if (context->csv.current_column >= context->csv.nums_field)
            {
                push_error(context, ERR_STRUCT_EXTRA_FIELD);
            }
            else if (!is_field_overflow)
            {
                context->csv.buffer[context->csv.length_field] = '\0';
                context->clbs.clb_write_to_arr(context);
            }
            
            except_delimeter(context);
            is_field_overflow = 0; // Сброс флага для следующего поля
        }
        else if (character == '\n')
        {
            if (context->csv.current_column <= context->csv.nums_field && !is_field_overflow)
            {
                context->csv.buffer[context->csv.length_field] = '\0';
                context->clbs.clb_write_to_arr(context);
            }

            // проверка на нехватку полей
            if (context->csv.current_column < context->csv.nums_field)
            {
                push_error(context, ERR_STRUCT_LESS_FIELDS);
            }
            // проверка на переизбыток полей
            else if (context->csv.current_column > context->csv.nums_field)
            {
                push_error(context, ERR_STRUCT_MORE_FIELDS);
            }
            
            except_newline(context);
            is_field_overflow = 0; 
            
            if (context->csv.current_row % 10000 == 0 && source->get_pos && context->clbs.clb_progress) 
            {
                context->clbs.clb_progress(source->get_pos(source->stream), context->file_size);
            }
        }
        else
        {
            // Накапливать символы только для разрешенных колонок и без переполнения буфера
            if (!is_field_overflow && context->csv.current_column <= context->csv.nums_field)
            {
                context->csv.buffer[context->csv.length_field++] = (char)character;
            }
        }
    }

    // финал файла
    if (context->csv.length_field > 0 && !is_field_overflow)
    {
        if (context->csv.current_column <= context->csv.nums_field)
        {
            context->csv.buffer[context->csv.length_field] = '\0';
            context->clbs.clb_write_to_arr(context);
        }
        
        // проверка на нехватку или избыток полей
        if (context->csv.current_column < context->csv.nums_field)
        {
            push_error(context, ERR_STRUCT_FINAL_STR_LESS_FIELD);
        }
        else if (context->csv.current_column > context->csv.nums_field)
        {
            push_error(context, ERR_FINAL_MORE_FIELD);
        }
        
        context->csv.current_row++;
    }
    // Если файл завершился пустым полем после ';'
    else if (context->csv.current_column > 0)
    {
        if (context->csv.current_column <= context->csv.nums_field)
        {
            push_error(context, ERR_FIND_LESS_FIELD);
        }
        else if (context->csv.current_column > context->csv.nums_field)
        {
            push_error(context, ERR_FIND_MORE_FIELD);
        }
    }
    
    if (source->get_pos && context->clbs.clb_progress)
    {
        context->clbs.clb_progress(source->get_pos(source->stream), context->file_size);
    }
    printf("\n");

    return context;
}


/**
 * @brief Вывод ошибок
 * @param *storage указатель на хранилище ошибок
 * @param rows кол-во строк для информативности
 */
void show_errors(struct IStorage *storage, size_t rows)
{
    size_t errnums = storage->size(storage);
    printf("Валидных строк:...%zu\n", rows);
    if (errnums > 0)
    {
        printf("Ошибок:...........%s%zu%s\n",RED, errnums, RESET);
        for (size_t i = 0; i < errnums; ++i)
        {
            struct ErrorParse *item_err = (struct ErrorParse*)storage->get(storage, i);
            printf(RED"%s"RESET, item_err->error_message);
        }
    }
    else
    {
        printf("Ошибок:...........%s%zu%s\n",GREEN, errnums, RESET);
    }
}















