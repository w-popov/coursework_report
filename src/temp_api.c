#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "temp_api.h"
#include "parser_csv.h"

/* --------------------- вывод статистики по каждому месяцу: ------------*/
/**
 * Среднемесячная температура
 */
int16_t average_monthly_temperature (struct TemperatureStats *tarr, size_t size, uint16_t month)
{
    if (tarr == NULL)
    {
        perror(RED "Error! zero pointer argument average_monthly_temperature()\n" RESET);
        return INT16_MIN;
    }
    if (month < 1 || month > 12)
    {
        perror(RED "Error! the month number must be between 1 and 12\n" RESET);
        return INT16_MIN;
    }
    long long int sum = 0;
    long long int count = 0;
    for (size_t i = 0; i < size; ++i)
    {
        if ((uint16_t)tarr[i].month == month)
        {
            sum += (int16_t)tarr[i].temperature;
            count++;
        }
    }
    if (count == 0)
    {
        return INT16_MIN; // данных за месяц нет
    }
    return (int16_t)(sum / count);
}

/**
 * Минимальная температура в текущем месяце
 */
int16_t min_temperature_current_month (struct TemperatureStats *tarr, size_t size, uint16_t month)
{
    if (tarr == NULL)
    {
        perror(RED "Error! zero pointer argument min_temperature_current_month()\n" RESET);
        return INT16_MIN;
    }
    if (month < 1 || month > 12)
    {
        perror(RED "Error! the month number must be between 1 and 12\n" RESET);
        return INT16_MIN;
    }
    int16_t min_temp = INT16_MAX;
    for (size_t i = 0; i < size; ++i)
    {
        if ((uint16_t)tarr[i].month == month)
        {
            if (tarr[i].temperature < min_temp)
            {
                min_temp = tarr[i].temperature;
            }
        }
    }
    return min_temp;
}

/**
 *  Максимальная температура в текущем месяце
 */
int16_t max_temperature_current_month (struct TemperatureStats *tarr, size_t size, uint16_t month)
{
    if (tarr == NULL)
    {
        perror(RED "Error! zero pointer argument max_temperature_current_month()\n" RESET);
        return INT16_MIN;
    }
    if (month < 1 || month > 12)
    {
        perror(RED "Error! the month number must be between 1 and 12\n" RESET);
        return INT16_MIN;
    }
    int16_t max_temp = INT16_MIN;
    for (size_t i = 0; i < size; ++i)
    {
        if ((uint16_t)tarr[i].month == month)
        {
            if (tarr[i].temperature > max_temp)
            {
                max_temp = tarr[i].temperature;
            }
        }
    }
    return max_temp;
}

/* --------------------- вывод статистики за год: ----------------------*/
/**
 * Среднегодовая температура
 */
int16_t average_annual_temperature (struct TemperatureStats *tarr, size_t size)
{
    if (tarr == NULL)
    {
        perror(RED "Error! zero pointer argument average_annual_temperature()\n" RESET);
        return INT16_MIN;
    }

    long long int sum = 0;
    long long int count = 0;
    for (size_t i = 0; i < size; ++i)
    {
        sum += (int16_t)tarr[i].temperature;
        count++;
    }
    if (count == 0)
    {
        return INT16_MIN;
    }
    return (int16_t)(sum / count);
}

/**
 *  Минимальная температура за год
 */
int16_t minimum_temperature (struct TemperatureStats *tarr, size_t size)
{
    if (tarr == NULL)
    {
        perror(RED "Error! zero pointer argument minimum_temperature()\n" RESET);
        return INT16_MIN;
    }

    int16_t min_temp = INT16_MAX;
    for (size_t i = 0; i < size; ++i)
    {
        if (tarr[i].temperature < min_temp)
        {
            min_temp = tarr[i].temperature;
        }
    }
    return min_temp;
}

/**
 *  Максимальная температура за год
 */
int16_t maximum_temperature (struct TemperatureStats *tarr, size_t size)
{
    if (tarr == NULL)
    {
        perror(RED "Error! zero pointer argument maximum_temperature()\n" RESET);
        return INT16_MIN;
    }

    int16_t max_temp = INT16_MIN;
    for (size_t i = 0; i < size; ++i)
    {
        if (tarr[i].temperature > max_temp)
        {
            max_temp = tarr[i].temperature;
        }
    }
    return max_temp;
}

struct Statistics calculate_statistics (struct TemperatureStats *tarr, size_t size)
{
    struct Statistics stats;
    memset(&stats, 0, sizeof(struct Statistics));
    stats.error = 0;

    if (!tarr)
    {
        stats.error = 1;
        return stats;
    }
    if (!size)
    {
        stats.error = 2;
        return stats;
    }

    for (int i = 0; i < MONTHS_SIZE; ++i)
    {
        struct MonthStats mon_stats;
        mon_stats.avg_temp = average_monthly_temperature(tarr, size, i + 1);
        mon_stats.max_temp = max_temperature_current_month(tarr, size, i + 1);
        mon_stats.min_temp = min_temperature_current_month(tarr, size, i + 1);

        if (mon_stats.avg_temp == INT16_MIN)
        {
            stats.error = 3;
        }

        stats.months[i] = mon_stats;
    }
    stats.avg_year = average_annual_temperature(tarr, size);
    stats.min_temp_year = minimum_temperature(tarr, size);
    stats.max_temp_year = maximum_temperature(tarr, size);
    return stats;
}

void show_statistics (struct TemperatureStats *tarr, size_t size, uint16_t month)
{
    const char *months[MONTHS_SIZE] = {"JAN", "FEB", "MAR", "APR",
                                       "MAY", "JUN", "JUL", "AUG",
                                       "SEP", "OCT", "NOV", "DEC"};

    struct Statistics stats = calculate_statistics(tarr, size);

    if (!month)
    {
        printf("\n+--------------+------------+------------+------------+\n");
        printf("| %-12s | %-10s | %-10s | %-10s |\n", 
            "Месяц       ", "Средняя T ", "Мин. T    ", "Макс. T   "
        );
        printf("+--------------+------------+------------+------------+\n");

        for (size_t i = 0; i < MONTHS_SIZE; ++i)
        {
            if (stats.months[i].avg_temp == INT16_MIN)
            {
                printf("| %-12s | %10s | %10s | %10s |\n", months[i], "-", "-","-");
            }
            else
            {
                printf("| %-12s | %10d | %10d | %10d |\n", months[i],
                       stats.months[i].avg_temp, stats.months[i].min_temp, stats.months[i].max_temp);
            }
        }
        printf("+--------------+------------+------------+------------+\n");
        printf("| %-12s | %10d | %10d | %10d |\n", 
              "YEAR TOTAL", stats.avg_year, stats.min_temp_year, stats.max_temp_year);
        printf("+--------------+------------+------------+------------+\n\n");
    }
    else
    {
        size_t idx = month - 1;
        if (idx < MONTHS_SIZE)
        {
            printf("\n+--------------+------------+------------+------------+\n");
            printf("| %-12s | %-10s | %-10s | %-10s |\n",             
                "Месяц       ", "Средняя T ", "Мин. T    ", "Макс. T   ");
            printf("+--------------+------------+------------+------------+\n");
            if (stats.months[idx].avg_temp == INT16_MIN)
            {
                printf("| %-12s | %10s | %10s | %10s |\n", months[idx], "-", "-", "-");
            }
            else
            {
                printf("| %-12s | %10d | %10d | %10d |\n",
                       months[idx],
                       stats.months[idx].avg_temp, stats.months[idx].min_temp,
                       stats.months[idx].max_temp);
            }
            printf("+--------------+------------+------------+------------+\n\n");
        }
    }
}

/**
 * Вывод массива температур
 */
void print_temperature_stats_array (struct TemperatureStats *tarr, size_t size)
{
    if (tarr == NULL)
    {
        perror(RED "Error! zero pointer argument print_temperature_stats_array()\n" RESET);
        return;
    }

    setvbuf(stdout, NULL, _IOFBF, 65536);
    for (size_t i = 0; i < size; ++i)
    {
        printf("%6zu]  Year: %4u,  Month: %2u,  Day: %2u,  Hours: %2u,  "
               "Minutes: %2u,  Temperature: %2d\n",
               i + 1, tarr[i].year, tarr[i].month, tarr[i].day, tarr[i].hours,
               tarr[i].minutes, tarr[i].temperature);
    }
    fflush(stdout);
}


/**
 * Функция сортировки массива температур по месяцу и температуре
 */
void sort_by_month_and_temp(struct TemperatureStats *tarr, size_t size, int sort_to)
{
    if (tarr == NULL || size == 0) 
    {
        return;
    }
    Comparator cmp = sort_to ? compare_by_month_and_temp_asc : compare_by_month_and_temp_desc;
    qsort(tarr, size, sizeof(struct TemperatureStats), cmp);
}


void show_help (void)
{
    printf("\n────────────────────────────────────────────────────────────\n");
    printf("Справка по использованию программы.\n");
    printf("Аргументы командной строки:\n");
    printf("  -h                Вызов этой справки\n");
    printf("  -f <filename.csv> Указать имя файла для парсинга (обязательно)\n");
    printf("  -m <номер месяца> Вывод статистики только за конкретный месяц (1-12)\n");
    printf("  -p                Вывод данных с файла на экран\n");
    printf("  -s <N>            Сортировка по месяцу и температуре.\n");
    printf("                    Где N = 1 по возрастанию, N = 0 по убыванию.\n");
    printf("                    Cортирует прочитанный из файла массив.\n");
    printf("                    Для вывода введите: <program> -f <file name> -p -s N\n");
    printf("─────────────────────────────────────────────────────────────\n\n");
}

/**
 * Экран открытия файла
 */
FILE *show_open_file_status (const char *file_name, int64_t *filesize)
{
    FILE *file = open_file(file_name, filesize);
    int64_t fsize = -1;

    #if defined(_WIN32) || defined(_WIN64)
    struct _stati64 statbuf;
    if (_stati64(file_name, &statbuf) == 0)
    {
        fsize = statbuf.st_size;
    }
    #else
    struct stat statbuf;
    if (stat(file_name, &statbuf) == 0)
    {
        fsize = statbuf.st_size;
    }
    #endif

    if (file == NULL || fsize == -1)
    {
        printf(RED_BOLD "Ошибка открытия файла: %s\n\n" RESET, file_name);
        return NULL;
    }

    *filesize = fsize;

    double display_size = (double)fsize;
    const char *unit = "b";
    const int64_t KB = 1024;
    const int64_t MB = 1024 * 1024;
    const int64_t GB = 1024 * 1024 * 1024;

    if (fsize >= GB)
    {
        display_size = (double)fsize / GB;
        unit = "Gb";
    }
    else if (fsize >= MB)
    {
        display_size = (double)fsize / MB;
        unit = "Mb";
    }
    else if (fsize >= KB)
    {
        display_size = (double)fsize / KB;
        unit = "Kb";
    }
    else
    {
        display_size = (double)fsize;
        unit = "b";
    }

    printf("══════════════════ Открытие файла .csv: ═══════════════════\n");
    printf("\tфайл:   %s\n", file_name);
    
    if (fsize < KB)
    {
        printf("\tразмер: %.0f %s\n", display_size, unit);
    }
    else
    {
        printf("\tразмер: %.2f %s\n", display_size, unit);
    }
    printf("═══════════════════════════════════════════════════════════\n");
    
    return file;
}

