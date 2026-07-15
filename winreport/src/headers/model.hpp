#pragma once

#include "parser_csv.h"
#include "interfaces.hpp"

namespace ModelApp
{

/**
 * Контекст парсинга
 */
class Context
{
  private:
    struct ContextParser context = {};              // Контекст СИ
    struct Csv csv = {};                            // Контекст для работы парсера СИ
    struct Callbacks clbs = {};                     // Обратные вызовы СИ
    struct IStorage_t *array = nullptr;             // Указатель на массив структук данных СИ
    struct IStorage_t *errors_parse = nullptr;      // Указатель на массив структур ошибок СИ
    size_t file_size = 0;                           // Размер файла

  public:
    Context();
    Context(const Context &)                = delete;
    Context(Context &&) noexcept            = delete;
    Context &operator=(const Context &)     = delete;
    Context &operator=(Context &&) noexcept = delete;

    inline const struct ContextParser *get_context() const 
    {
        return &context;
    }

    inline const char *get_delimiter () const
    {
        return csv.delimiter;
    }

    inline void set_delimiter (const char *delim)
    {
        csv.delimiter = delim;
    }

    inline uint16_t get_nums_field () const
    {
        return csv.nums_field;
    }

    inline void set_nums_field (uint16_t nums)
    {
        csv.nums_field = nums;
    }

    // set callback записи в массив структур
    inline void set_callback_write (CallbackWriteToArray clb_w)
    {
        context.clbs.clb_write_to_arr = clb_w;
    }

    // set callback прогрессбара
    inline void set_callback_progressbar (CallbackProgressBar clb_progress)
    {
        context.clbs.clb_progress = clb_progress;
    }

    inline IStorage_t *get_array () const
    {
        return context.array;
    }

    inline void set_array (IStorage_t *arr)
    {
        context.array = arr;
    }

    inline IStorage_t *get_errors_parse () const
    {
        return context.errors_parse;
    }

    inline void set_errors_parse (IStorage_t *errors)
    {
        context.errors_parse = errors;
    }
    
    inline size_t get_file_size () const
    {
        return context.file_size;
    }

    inline void set_file_size (size_t size)
    {
        context.file_size = size;
    }
};

//////////////////// Базовый класс для модели/ей ////////////////////////////////////////
/**
 * Добавить подписчика
 */
void InterfacesApp::Observable::add_observer (Observer *o)
{
    if (o)
    {
        observers.push_back(o);
    }
};

/**
 * Убрать подписчика
 */
void InterfacesApp::Observable::remove_observer (Observer *o)
{
    observers.erase(
        std::remove(observers.begin(), observers.end(), o), observers.end()
    );
};

/**
 * Уведомить всех подписчиков об изменении.
 * protected. Метод доступен только наследникам (например, FileModel)
 */
void InterfacesApp::Observable::notify_observers 
(const std::any &data, const AppError* error = nullptr)
{
    for (auto *obs : observers)
    {
        obs->on_model_changed(data, error);
    }
};


} // namespace ModelApp
