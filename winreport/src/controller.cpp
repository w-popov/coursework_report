
#include "controller.hpp"
#include "view.hpp"
#include "model.hpp"
#include <FL/fl_ask.H>

namespace ControllerApp
{

AppController::AppController
(ViewApp::AppWindow *window, ModelApp::ModelParse *model_parse) : view(window), model(model_parse)
{
    if (window && model_parse)
    {
        /* Связывание Си-указателя прогресса в модели напрямую с
           методом окна через Си-лямбду */
        static ViewApp::AppWindow *static_view = window;
        model_parse->set_callback_progressbar(
            [] (int64_t current, int64_t total) {
                if (static_view)
                {
                    static_view->update_progress_bar_value(current, total);
                }
            });

        model_parse->add_observer(this);

        // Настройка события кнопок окна
        window->on_file_selected = [] (const char *path, void *context)
        {
            auto *m = static_cast<ModelApp::ModelParse *>(context);
            if (m && path)
            {
                m->set_file_path(path);
            }
        };

        window->on_start_parsing = [] (void *context)
        {
            auto *m = static_cast<ModelApp::ModelParse *>(context);
            if (m)
            {
                const char *path = m->get_file_path();
                if (path && path[0] != '\0')
                {
                    m->parsing(path);
                }
                else
                {
                    fl_alert("Внимание: Сначала выберите файл через кнопку 'Файл'!");
                }
            }
        };

        window->callback_context = model_parse;
    }
}

AppController::~AppController()
{
    if (model)
    {
        model->set_callback_progressbar(nullptr);
        model->remove_observer(this);
    }
}

void AppController::update(const std::any &data, InterfacesApp::DataType datatype)
{
    if (!view)
    {
        return;
    }

    switch (datatype)
    {
    case InterfacesApp::DataType::FullRowsList:

        break;

    case InterfacesApp::DataType::ErrorsParse:
        view->update_progress_bar_value(100, 100);

        break;

    case InterfacesApp::DataType::ErrorOpenFile:
        fl_alert("Ошибка: Не удалось открыть выбранный CSV файл!");
        break;

    case InterfacesApp::DataType::ErrorNullParse:
        fl_alert("Критическая ошибка: Внутренний сбой структуры парсера!");
        break;

    case InterfacesApp::DataType::AverageTable:
        break;
    }
}

} // namespace ControllerApp
