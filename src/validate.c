#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include "temp_api.h"
#include "parser_csv.h"

int is_valid_number(char buffer[], int is_signed)
{
    if (buffer == NULL)
    {
        return 0;
    }

    size_t i = 0;

    while (buffer[i] != '\0' && isspace((unsigned char)buffer[i]))
    {
        i++;
    }
    if (buffer[i] == '\0')
    {
        return 0;
    }
    if (buffer[i] == '-' || buffer[i] == '+')
    {
        if (!is_signed)
        {
            return 0;
        }

        i++;
        
        if (buffer[i] == '\0' || isspace((unsigned char)buffer[i]))
        {
            return 0;
        }
    }
    if (!isdigit((unsigned char)buffer[i]))
    {
        return 0;
    }
    while (buffer[i] != '\0' && isdigit((unsigned char)buffer[i]))
    {
        i++;
    }
    while (buffer[i] != '\0' && isspace((unsigned char)buffer[i]))
    {
        i++;
    }

    return (buffer[i] == '\0');
}


int validator_fields (long value, uint16_t num_field)
{    
    switch (num_field)
    {
    case 0:
        return (uint16_t)value > 1950 && (uint16_t)value < 2040;
    case 1:
        return (uint16_t)value > 0 && (uint16_t)value < 13;
    case 2:
        return (uint16_t)value > 0 && (uint16_t)value < 32;
    case 3:
        return (uint16_t)value < 24;
    case 4:
        return (uint16_t)value < 60;
    case 5:
        return (int16_t)value > -99 && (int16_t)value < 100;
    default:
        break;
    }
    return 0;
}

/**
 * Прогрессбар во время парсинга файла
 */
void print_progress_bar(int64_t current, int64_t total)
{
    if (total <= 0)
    {
        return;
    }

    // процент выполнения
    int percentage = (int)((current * 100) / total);
    
    // ширина полосы индикатора (количество символов '=' внутри [ ])
    const int bar_width = 30;
    int progress_position = (int)((current * bar_width) / total);

    printf(BLUE"\rПарсинг: ["RESET);
    for (int i = 0; i < bar_width; ++i)
    {
        if (i < progress_position)
        {
            printf(BLUE"="RESET);
        }
        else if (i == progress_position)
        {
            printf(BLUE">"RESET);
        }
        else
            printf(" ");
    }
    printf(BLUE"] %3d%%"RESET, percentage);
    fflush(stdout); 
}


int write_to_array (struct ContextParser *context)
{
    context->csv.buffer[context->csv.length_field] = '\0';
    
    static struct TemperatureStats current;
    static int is_row_valid = 1; // Флаг валидности всей текущей строки

    if (context->csv.current_column == 0)
    {
        current = (struct TemperatureStats){0};
        is_row_valid = 1;
    }
    context->csv.buffer[context->csv.length_field] = '\0';
    int is_field_valid = 1;
    char *endptr;
    
    if (context->csv.length_field == 0)
    {
        is_field_valid = 0;
    }
    else if (context->csv.current_column >= 0 && context->csv.current_column <= context->csv.nums_field)
    {
        long value = strtol(context->csv.buffer, &endptr, 10);
        
        if (*endptr != '\0' && !isspace((unsigned char)*endptr))
        {
            is_field_valid = 0;
        }
        
        // Валидация конкретной колонки
        if (context->csv.current_column == 0)
        {
            if (is_field_valid && is_valid_number(context->csv.buffer, 0) && 
                validator_fields(value, context->csv.current_column))
            {
                current.year = value;
            }
            else
            {
                is_field_valid = 0;
            }
        }
        else if (context->csv.current_column == 1)
        {
            if (is_field_valid && is_valid_number(context->csv.buffer, 0) && 
                validator_fields(value, context->csv.current_column))
            {
                current.month = value;
            }
            else
            {
                is_field_valid = 0;
            }
        }
        else if (context->csv.current_column == 2)
        {
            if (is_field_valid && is_valid_number(context->csv.buffer, 0) && 
                validator_fields(value, context->csv.current_column))
            {
                current.day = value;
            }
            else
            {
                is_field_valid = 0;
            }
        }
        else if (context->csv.current_column == 3)
        {
            if (is_field_valid && is_valid_number(context->csv.buffer, 0) && 
                validator_fields(value, context->csv.current_column))
            {
                current.hours = value;
            }
            else
            {
                is_field_valid = 0;
            }
        }
        else if (context->csv.current_column == 4)
        {
            if (is_field_valid && is_valid_number(context->csv.buffer, 0) && 
                validator_fields(value, context->csv.current_column))
            {
                current.minutes = value;
            }
            else
            {
                is_field_valid = 0;
            }
        }
        else if (context->csv.current_column == 5)
        {
            if (is_field_valid && is_valid_number(context->csv.buffer, 1) && 
                validator_fields(value, context->csv.current_column))
            {
                current.temperature = (int16_t)value;
            }
            else
            {
                is_field_valid = 0;
            }
        }
    }
    else
    {
        is_field_valid = 0;
    } 
    if (!is_field_valid)
    {
        is_row_valid = 0;
        push_error(context, ERR_VALIDATE);
    }
    if (context->csv.current_column == context->csv.nums_field)
    {
        if (is_row_valid)
        {
            context->array->push(context->array, &current);
            //svector_push(context->array, &current);
        }
        
        // сброс для след. строки
        current = (struct TemperatureStats){0}; 
        is_row_valid = 1;                       
    }
    
    return is_field_valid;
}

// Компараторы qsort --------------------------------------------------------

int compare_by_month_and_temp_asc(const void *a, const void *b) 
{
    const struct TemperatureStats *ia = (const struct TemperatureStats *)a;
    const struct TemperatureStats *ib = (const struct TemperatureStats *)b;
    
    if (ia->month != ib->month) 
    {
        return (ia->month > ib->month) - (ia->month < ib->month);
    }
    
    return (ia->temperature > ib->temperature) - (ia->temperature < ib->temperature);
}

int compare_by_month_and_temp_desc(const void *a, const void *b) 
{
    const struct TemperatureStats *ia = (const struct TemperatureStats *)a;
    const struct TemperatureStats *ib = (const struct TemperatureStats *)b;
    
    if (ia->month != ib->month) 
    {
        return (ia->month < ib->month) - (ia->month > ib->month);
    }
    
    return (ia->temperature < ib->temperature) - (ia->temperature > ib->temperature);
}