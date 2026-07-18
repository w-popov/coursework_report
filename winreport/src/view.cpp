#include "view.hpp"
#include "interfaces.hpp"
#include "temp_api.h"
#include "parser_csv.h"
#include <FL/Fl.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <filesystem>
#include <cstdio>

namespace ViewApp
{

DataTable::DataTable(int X, int Y, int W, int H, const char *L) 
    : Fl_Table(X, Y, W, H, L) 
{ 
    rows(0);
    cols(7);
    col_header(1);      
    scrollbar_size(16); 
    row_height_all(25);  
    col_width_all(80);  
    type(Fl_Scroll::BOTH);
    end();
};

void ViewApp::DataTable::set_data(IStorage_t* data)
{
    datasource = data;
    if (datasource)
    {
        rows(datasource->size(datasource));
    } 
    else
    {
        rows(0);
    }
    this->recalc_dimensions(); 
    this->init_sizes(); 
    this->redraw(); 
};

void DataTable::resize(int X, int Y, int W, int H)
{
    Fl_Table::resize(X, Y, W, H); 
    this->table_resized();  
};

void DataTable::draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H)
{
    static char buffer[64];

    switch (context)
    {
        case CONTEXT_STARTPAGE: 
            fl_font(FL_HELVETICA, 12);
            break;

        case CONTEXT_COL_HEADER: 
            fl_push_clip(X, Y, W, H);
            fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, FL_BACKGROUND_COLOR);
            fl_color(FL_BLACK);
            if (C == 0) fl_draw("№", X, Y, W, H, FL_ALIGN_CENTER);
            if (C == 1) fl_draw("Год", X, Y, W, H, FL_ALIGN_CENTER);
            if (C == 2) fl_draw("Месяц", X, Y, W, H, FL_ALIGN_CENTER);
            if (C == 3) fl_draw("День", X, Y, W, H, FL_ALIGN_CENTER);
            if (C == 4) fl_draw("Час", X, Y, W, H, FL_ALIGN_CENTER);
            if (C == 5) fl_draw("Мин.", X, Y, W, H, FL_ALIGN_CENTER);
            if (C == 6) fl_draw("T (°C)", X, Y, W, H, FL_ALIGN_CENTER);
            
            fl_pop_clip();
            break;

        case CONTEXT_CELL: 
        { 
            fl_push_clip(X, Y, W, H);
            fl_color(FL_WHITE);
            fl_rectf(X, Y, W, H);
            fl_color(FL_LIGHT2); 
            fl_rect(X, Y, W, H);
            fl_color(FL_BLACK); 
            if (datasource) 
            {
                struct TemperatureStats* tarr = (struct TemperatureStats*)datasource->raw_data(datasource);
                if (tarr && R < (int)datasource->size(datasource)) 
                {
                    buffer[0] = '\0'; 
                    if (C == 0) sprintf(buffer, "%d", R + 1);
                    if (C == 1) sprintf(buffer, "%u", tarr[R].year); 
                    if (C == 2) sprintf(buffer, "%u", tarr[R].month);
                    if (C == 3) sprintf(buffer, "%u", tarr[R].day); 
                    if (C == 4) sprintf(buffer, "%u", tarr[R].hours); 
                    if (C == 5) sprintf(buffer, "%u", tarr[R].minutes);
                    if (C == 6) sprintf(buffer, "%d", tarr[R].temperature); 

                    fl_draw(buffer, X + 4, Y, W - 8, H, FL_ALIGN_LEFT);
                }
            }
            fl_pop_clip();
            break;
        }
        default:
            break;
    }
};


AppWindow::AppWindow(int width, int height, const char *title)
    : Fl_Double_Window(width, height, title)
{
    this->box(FL_FLAT_BOX);
    this->color(FL_WHITE);
    this->selection_color(FL_BACKGROUND_COLOR);
    this->labeltype(FL_NO_LABEL);
    this->align(FL_ALIGN_TOP);

    init_layout();
};

void AppWindow::init_layout()
{
    main_flex = new Fl_Flex(0, 0, this->w(), this->h());
    main_flex->type(Fl_Flex::VERTICAL); 
    main_flex->margin(10, 10, 10, 10);
    main_flex->gap(10);

    {
        flex_top_buttons = new Fl_Flex(0, 0, 0, 40);
        flex_top_buttons->type(Fl_Flex::HORIZONTAL);

        btn_open_file = new Fl_Button(0, 0, 80, 40, "Файл");
        btn_open_file->labelsize(12);

        btn_parse_csv = new Fl_Button(0, 0, 80, 40, "Парсинг");
        btn_parse_csv->labelsize(12);

        btn_report = new Fl_Button(0, 0, 80, 40, "Отчет");
        btn_report->labelsize(12);

        btn_all = new Fl_Button(0, 0, 80, 40, "Данные");
        btn_all->labelsize(12);

        btn_save = new Fl_Button(0, 0, 80, 40, "В .html");
        btn_save->labelsize(12);

        btn_exit = new Fl_Button(0, 0, 80, 40, "Выход");
        btn_exit->labelsize(12);

        flex_top_buttons->gap(10);
        flex_top_buttons->fixed(btn_open_file, 80);
        flex_top_buttons->fixed(btn_parse_csv, 80);
        flex_top_buttons->fixed(btn_report, 80);
        flex_top_buttons->fixed(btn_all, 80);
        flex_top_buttons->fixed(btn_save, 80);
        flex_top_buttons->fixed(btn_exit, 80);

        flex_top_buttons->end();
    }

    {
        flex_errors = new Fl_Flex(0, 0, 0, 100);
        browse_errs = new Fl_Browser(0, 0, 0, 100);
        browse_errs->textsize(12);
        flex_errors->end();
        flex_errors->hide();
    }

    {
        flex_table_container = new Fl_Flex(0, 0, 100, 100);
        flex_table_container->type(Fl_Flex::VERTICAL);
        flex_table_container->margin(0, 0, 0, 0);
        
        table_parse = new DataTable(0, 0, 100, 100); 
        table_parse->box(FL_FLAT_BOX);
        table_parse->color(FL_WHITE);
        
        table_parse->hide();
        table_parse->end(); 
        
        flex_table_container->end();
    }

    {
        flex_bottom_status = new Fl_Flex(0, 0, 0, 45);
        flex_bottom_status->gap(5);

        lbl_file_name = new Fl_Box(0, 0, 0, 15, "Файл не выбран");
        lbl_file_name->align(FL_ALIGN_LEFT | FL_ALIGN_INSIDE);
        lbl_file_name->labelsize(12);

        progress_bar = new Fl_Progress(0, 0, 0, 20);
        progress_bar->minimum(0.0f);
        progress_bar->maximum(100.0f);
        progress_bar->value(0.0f);
        progress_bar->selection_color(fl_rgb_color(87, 179, 222));
        progress_bar->labelsize(12);
        progress_bar->hide();
        flex_bottom_status->end();
    }

    main_flex->fixed(flex_top_buttons, 40);
    main_flex->fixed(flex_errors, 100);
    main_flex->fixed(flex_bottom_status, 45);
    main_flex->resizable(flex_table_container);
    main_flex->end();

    this->resizable(main_flex);
    this->end();
    main_flex->layout();
}

//////////////////////////// View //////////////////////////////////////////
View::View(Fl_Double_Window *w) : window(static_cast<AppWindow*>(w))
{
    window->btn_open_file->callback(clb_open_file, this);
    window->btn_parse_csv->callback(clb_parse_csv, this);
    window->btn_all->callback(clb_all, this);
}

void View::update(const std::any &data, InterfacesApp::DataType datatype)
{
    switch (datatype)
    {
    case InterfacesApp::DataType::FullRowsList:
        try 
        {
            current_dataset = std::any_cast<IStorage_t*>(data);
        }
        catch (const std::bad_any_cast& e) 
        {
            current_dataset = nullptr;
        }
        break;

    case InterfacesApp::DataType::ErrorsParse:
        update_progress_bar_value(100, 100);
        try 
        {
            auto errors = std::any_cast<IStorage_t*>(data);
            size_t size = errors->size(errors);
            window->browse_errs->clear();
            if (size > 0)
            {
                window->flex_errors->show();
                window->main_flex->layout();
                for (size_t i = 0; i < size; ++i)
                {
                    struct ErrorParse *item_err = (struct ErrorParse *)errors->get(errors, i);
                    window->browse_errs->add(item_err->error_message);
                    window->browse_errs->redraw();
                }
            }

        } 
        catch (const std::bad_any_cast& e) 
        {
            printf("Type bad error\n");
        }
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
};

void View::handle_open_file()
{
    Fl_Native_File_Chooser fnfc;
    fnfc.title("Выберите CSV файл");
    fnfc.type(Fl_Native_File_Chooser::BROWSE_FILE);
    fnfc.filter("CSV Файлы\t*.csv");

    if (fnfc.show() == 0 && fnfc.filename() != nullptr)
    {
        const char *path = fnfc.filename();
        std::string short_name =
            std::filesystem::path(path).filename().string();

        set_status_file(short_name.c_str());
        update_progress_bar_value(0, 100);
        
        if (on_file_selected)
        {
            on_file_selected(path);
        }
    }
};

void View::handle_print_all()
{
    if (window && current_dataset && current_dataset->size(current_dataset) > 0)
    {
        window->flex_table_container->show();
        window->table_parse->show();
        window->main_flex->layout(); 
        window->table_parse->set_data(current_dataset);
    }
    else
    {
        window->table_parse->set_data(nullptr);
        fl_alert("Нет данных для отображения! Сначала выберите файл и запустите парсинг.");
    }
};

void View::handle_parse_csv()
{
    if (on_start_parsing)
    {
        on_start_parsing();
    }
};

void View::set_status_file(const char *name)
{
    if (window->lbl_file_name)
    {
        window->lbl_file_name->copy_label(name);
        window->lbl_file_name->redraw();
    }
};

void View::update_progress_bar_value(int64_t current, int64_t total)
{
    if (!window->progress_bar || total <= 0)
    {
        return;
    }

    float percentage = (static_cast<float>(current) * 100.0f) / static_cast<float>(total);
    if (percentage > 100.0f)
    {
        percentage = 100.0f;
    }

    window->progress_bar->value(percentage);

    char pct_text[32];
    snprintf(pct_text, sizeof(pct_text), "Парсинг: %.1f%%", percentage);
    window->progress_bar->copy_label(pct_text);

    window->progress_bar->redraw();
    Fl::check();
};

void View::show_widget(Fl_Widget* w, Fl_Flex* f)
{
    if (window)
    {   w->show();
        f->layout();
        window->redraw();
    }
    Fl::flush();
}

void View::hide_widget(Fl_Widget* w, Fl_Flex* f)
{
    if (window)
    {   w->hide();
        f->layout();
        window->redraw();
    }
    Fl::flush();
}

} // namespace ViewApp
