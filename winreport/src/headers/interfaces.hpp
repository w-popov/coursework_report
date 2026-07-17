#pragma once

#include <any>
#include <vector>
#include <algorithm>

namespace InterfacesApp
{

/**
 * Тип данных для подписчиков
 */
enum class DataType
{
    ErrorsParse,        // Данные для отображения ошибок
    FullRowsList,       // Данные для показа списка строк
    AverageTable,       // Данные для таблицы средних значений
    ErrorNullParse,     // Функция парсера вернула NULL
    ErrorOpenFile,      // Ошибка открытия файла
};

/**
 * Наблюдатель
 */
class Observer
{
  public:
    virtual ~Observer() = default;
    virtual void update (const std::any &data, DataType datatype) = 0;
};


} // namespace InterfacesApp
