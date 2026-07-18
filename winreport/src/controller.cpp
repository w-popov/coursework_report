
#include "controller.hpp"
#include "view.hpp"
#include "model.hpp"
#include <FL/fl_ask.H>

namespace ControllerApp
{
// static
void AppController::progress_bridge(void* self, int64_t current, int64_t total)
{
    auto* current_view = static_cast<ViewApp::View*>(self);
    
    if (current_view)
    {
        current_view->update_progress_bar_value(current, total);
    }
}

// конструктор
AppController::AppController
(ViewApp::View *v, ModelApp::ModelParse *model) : view(v), model(model)
{
    if (view && model)
    {
        model->add_observer(view);
        model->set_callback_context(view);
        model->set_callback_progressbar(progress_bridge);

        // колбэк : отдать модели путь выбранного файла
        view->on_file_selected = [model](const char *path)
        {
            if (model && path)
            {
                model->set_file_path(path);
            }
        };

        // колбэк: запуск парсинга
        view->on_start_parsing = [model, v]()
        {
            if (v && v->window)
            {
                v->show_widget(v->window->progress_bar, v->window->flex_bottom_status);
            } 
            if (model)
            {
                const char *path = model->get_file_path(); 
                if (path && path[0] != '\0')
                {
                    v->clear_table_parse();
                    v->clear_table_stats();
                    v->window->btn_parse_csv->deactivate();
                    model->parsing(path);
                    v->window->btn_parse_csv->activate();
                }
                else
                {
                    fl_alert("Внимание: Сначала выберите файл через кнопку 'Файл'!");
                }
            }
        };

        // колбэк получить статистику за год/месяц
        view->on_start_stats = [model]()
        {
            if (model)
            {
                model->statistics();
            }
        };
    }
}

AppController::~AppController()
{
    if (model) 
    {
        model->set_callback_progressbar(nullptr);
        model->set_callback_context(nullptr);
    }
};



} // namespace ControllerApp
