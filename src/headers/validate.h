#ifndef _VALIDATE_H_
#define _VALIDATE_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Провека что из строки можно сделать число
 * @param buffer массив со значением столбца .csv
 * @param is_signed число знаковое, или беззнаковое
 * @return bool
 */
int is_valid_number(char buffer[], int is_signed);


/**
 * @brief Валидатор конкретного случая для 5 столбцов
 * @param value преобразованное из стоки чило
 * @param num_field номер столбца
 */
int validator_fields (long value, uint16_t num_field);


/**
 * Прогрессбар во время парсинга файла
 */
void print_progress_bar(int64_t current, int64_t total);


/**
 * @brief Основная проверка столбцов. Запись валидных в массив 
 *        структур температур и/или запись ошибок валидации
 * @param *context контекст парсинга
 */
int write_to_array (struct ContextParser *context);


#ifdef __cplusplus
}
#endif

#endif

