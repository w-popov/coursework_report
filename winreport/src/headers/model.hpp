#pragma once

#include <string>
#include <vector>
#include <any>
#include "interfaces.hpp"
#include "temp_api.h"
#include "parser_csv.h"

namespace ModelApp
{

/**
 * Базовый класс для источников данных (Model)
 */
class Observable
{
  private:
    std::vector<InterfacesApp::Observer *> observers;

  public:
    virtual ~Observable() = default;

    /**
     * Добавить подписчика
     */
    inline void add_observer (InterfacesApp::Observer *o)
    {
        if (o)
        {
            observers.push_back(o);
        }
    }

    /**
     * Убрать подписчика
     */
    inline void remove_observer (InterfacesApp::Observer *o)
    {
        observers.erase(std::remove(observers.begin(), observers.end(), o),
                        observers.end());
    }

  protected:
    /**
     * Уведомить всех подписчиков об изменении.
     * protected. Метод доступен только наследникам
     */
    inline void notify (const std::any &data, InterfacesApp::DataType datatype)
    {
        for (auto *obs : observers)
        {
            obs->update(data, datatype);
        }
    }
};

/**
 * Основная модель парсинга
 */
class ModelParse : public Observable
{
  private:
    int64_t filesize = -1;
    struct SVector array_vec;
    struct SVector err_array_vec;
    struct IStorage_t *array = nullptr;
    struct IStorage_t *errors_array = nullptr;
    struct ContextParser *context = nullptr;
    struct ParseSource *parse_source = nullptr;
    struct Statistics stats = {};
    struct Statistics *pstats = nullptr;
    std::string file_path;

  public:
    ~ModelParse();
    ModelParse();
    ModelParse(const ModelParse &) = delete;
    ModelParse(ModelParse &&) noexcept = delete;
    ModelParse &operator=(const ModelParse &) = delete;
    ModelParse &operator=(ModelParse &&) noexcept = delete;

    void parsing (const char *filename);

    inline void set_file_path (const char *path)
    {
        file_path = path;
    }

    inline void set_callback_progressbar (CallbackProgressBar clb_progress)
    {
        if (context)
        {
            context->clbs.clb_progress = clb_progress;
        }
    }

    // Запись C++ адреса (View) в Си-структуру
    inline void set_callback_context(void* user_data)
    {
        if (context)
        {
            context->clbs.self = user_data;
        }
    }

    inline const char *get_delimiter () const
    {
        return context ? context->csv.delimiter : nullptr;
    }

    inline void set_delimiter (const char *delim)
    {
        if (context)
        {
            context->csv.delimiter = delim;
        }
    }

    inline uint16_t get_nums_field () const
    {
        return context ? context->csv.nums_field : 0;
    }

    inline void set_nums_field (uint16_t nums)
    {
        if (context)
        {
            context->csv.nums_field = nums;
        }
    }

    // set callback записи в массив структур
    inline void set_callback_write (CallbackWriteToArray clb_w)
    {
        if (context)
        {
            context->clbs.clb_write_to_arr = clb_w;
        }
    }

    inline IStorage_t *get_array () const
    {
        return context ? context->array : nullptr;
    }

    inline void set_array (IStorage_t *arr)
    {
        if (context)
        {
            context->array = arr;
        }
    }

    inline IStorage_t *get_errors_parse () const
    {
        return context ? context->errors_parse : nullptr;
    }

    inline void set_errors_parse (IStorage_t *errors)
    {
        if (context)
        {
            context->errors_parse = errors;
        }
    }

    inline int64_t get_file_size () const
    {
        return context ? context->file_size : 0;
    }

    inline void set_file_size (int64_t size)
    {
        if (context)
        {
            context->file_size = size;
        }
    }

    inline const char *get_file_path () const
    {
        return file_path.c_str();
    }

    void model_init();
    void model_free();
    void storages_clear();
    void statistics();
};

} // namespace ModelApp
