#ifndef _TEMP_API_H_
#define _TEMP_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#if defined(_WIN32) || defined(_WIN64)
#include <sys/stat.h>
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif

#define MAX_SIZE_ARRAY  1000000     // Максимальный размер массива температур
#define FILE_NAME_PATH  2000        // Макс. размер пути/имени файла
#define MONTHS_SIZE     12          // Размер массива месяцев

/**
 * Структура данных из файла
 */
struct TemperatureStats
{
    uint32_t year       :11;
    uint32_t month      :4;
    uint32_t day        :6;
    uint32_t hours      :5;
    uint32_t minutes    :6;
    int16_t temperature;
};

/**
 * Статистика за месяц
 */
struct MonthStats
{
    int16_t avg_temp;         // среднемесячная температура
    int16_t min_temp;         // минимальная температура в текущем месяце
    int16_t max_temp;         // максимальная температура в текущем месяце
};

/**
 * Структура статистики
 */
struct Statistics
{
    struct MonthStats months[MONTHS_SIZE];  // cтатистика за каждый месяц
    int32_t avg_year       : 8;             // среднегодовая температура
    int32_t min_temp_year  : 8;             // минимальная годовая температура
    int32_t max_temp_year  : 8;             // максимальная годовая температура
    int32_t error          : 8;             // наличие ошибки
};


/* --------------------- вывод статистики по каждому месяцу: */ 
/* Среднемесячная температура */
int16_t average_monthly_temperature (struct TemperatureStats*, size_t, uint16_t);

/* Минимальная температура в текущем месяце */
int16_t min_temperature_current_month (struct TemperatureStats*, size_t, uint16_t);

/* Максимальная температура в текущем месяце */
int16_t max_temperature_current_month (struct TemperatureStats*, size_t, uint16_t);

/* --------------------- вывод статистику за год: */
/* Среднегодовая температура */
int16_t average_annual_temperature (struct TemperatureStats*, size_t);

/* Минимальная температура */
int16_t minimum_temperature (struct TemperatureStats*, size_t);

/* Максимальная температура */
int16_t maximum_temperature (struct TemperatureStats*, size_t);

/* Вывод массива температур */
void print_temperature_stats_array (struct TemperatureStats*, size_t size);


/**
 * @brief Расчет статистики
 * @param *tarr массив структур температур
 * @param size размер массива
 * @return структура с вычисленными значениями статистики
 */
struct Statistics calculate_statistics (struct TemperatureStats *tarr, size_t size);

/**
 * @brief Компаратор ф-ции сортировки
 */
typedef int (*Comparator)(const void *a, const void *b);

/**
 * @brief Компаратор qsort. Сравнение месяцы и температура по возрастанию
 */
int compare_by_month_and_temp_asc(const void *a, const void *b);

/**
 * @brief Компаратор qsort. Сравнение месяцы и температура по убыванию
 */
int compare_by_month_and_temp_desc(const void *a, const void *b);

/**
 * @brief Функция сортировки массива температур по месяцу и температуре 
 */
void sort_by_month_and_temp(struct TemperatureStats *tarr, size_t size, int sort_to);

/**
 * @brief вывод статистики на экран
 * @param *tarr массив структур температур
 * @param size размер массива
 * @param month если 0 < month <= 12, то выводится статистика за месяц,
 *              если month == 0, то выводится статистика за каждый месяц.
 */
void show_statistics (struct TemperatureStats *tarr, size_t size, uint16_t month);

/* Меню вывода статистики */
void show_menu (void);

/* Меню выбора файла */
FILE* show_open_file_status (const char *file_name, int64_t *filesize);

/**
 * Вывод справки
 */
void show_help(void);

#ifdef __cplusplus
}
#endif

#endif