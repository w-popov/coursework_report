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

StatsTable::StatsTable(int X, int Y, int W, int H, const char *L)
    : Fl_Table(X, Y, W, H, L)
{
    rows(0);
    cols(4);
    col_header(1);      
    col_header_height(30); 
    type(Fl_Scroll::BOTH);  
    scrollbar_size(16);
    row_height_all(35);  
    col_width_all(120);

    begin();
    box(FL_THIN_DOWN_FRAME);
    color(FL_WHITE);
    selection_color(fl_rgb_color(51, 153, 255));
    end();
    
    init_sizes();
};

void StatsTable::set_data(struct Statistics *data)
{
    datasource = data;
    rows(13);
    this->init_sizes(); 
    this->recalc_dimensions(); 
    this->redraw();
};

void StatsTable::draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H)
{
    static const char* month_names[] = {
        "JAN", "FEB", "MAR", "APR", "MAY", "JUN",
        "JUL", "AUG", "SEP", "OCT", "NOV", "DEC"
    };
    column_headers = {"Месяц", "Средняя T(°C)", "Мин. T(°C)", "Макс. T(°C)"};
    static char buffer[32];

    switch (context)
    {
        case CONTEXT_STARTPAGE: 
            fl_font(FL_HELVETICA, 12);
            break;

        case CONTEXT_COL_HEADER: 
        {
            if (W <= 0 || H <= 0) break;
            
            fl_push_clip(X, Y, W, H);
            fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, fl_rgb_color(200, 220, 255));
            fl_color(FL_DARK_BLUE);
            fl_font(FL_HELVETICA_BOLD, 12);
            
            if (C >= 0 && C < (int)column_headers.size())
            {
                fl_draw(column_headers[C].c_str(), X, Y, W, H, FL_ALIGN_CENTER);
            }
            
            fl_pop_clip();
            break;
        }

        case CONTEXT_CELL: 
        { 
            if (R < 0 || C < 0 || R >= rows() || C >= cols() || W <= 0 || H <= 0) {
                break;
            }
            
            fl_push_clip(X, Y, W, H);
            
            bool is_total = (R == 12);
            
            fl_color(FL_WHITE);
            fl_rectf(X, Y, W, H);
            
            // Сетка
            fl_color(fl_rgb_color(180, 200, 230));
            fl_line_style(FL_SOLID, is_total ? 2 : 1);
            fl_rect(X, Y, W, H);
            fl_line_style(FL_SOLID, 1);
            
            buffer[0] = '\0';
            
            if (is_total)
            {
                // Итоговая строка
                switch(C) 
                {
                    case 0: snprintf(buffer, sizeof(buffer), "YEAR TOTAL"); break;
                    case 1: snprintf(buffer, sizeof(buffer), "%d", datasource->avg_year); break;
                    case 2: snprintf(buffer, sizeof(buffer), "%d", datasource->min_temp_year); break;
                    case 3: snprintf(buffer, sizeof(buffer), "%d", datasource->max_temp_year); break;
                }
                fl_font(FL_HELVETICA_BOLD, 12);
                fl_color(FL_DARK_BLUE);
            } 
            else 
            {
                // Данные за месяц
                switch(C) {
                    case 0: 
                        snprintf(buffer, sizeof(buffer), "%s", month_names[R]); 
                        break;
                    case 1: 
                        if (datasource->months[R].avg_temp == INT16_MIN)
                            snprintf(buffer, sizeof(buffer), "-");
                        else
                            snprintf(buffer, sizeof(buffer), "%d", datasource->months[R].avg_temp);
                        break;
                    case 2: 
                        if (datasource->months[R].min_temp == INT16_MIN)
                            snprintf(buffer, sizeof(buffer), "-");
                        else
                            snprintf(buffer, sizeof(buffer), "%d", datasource->months[R].min_temp);
                        break;
                    case 3: 
                        if (datasource->months[R].max_temp == INT16_MIN)
                            snprintf(buffer, sizeof(buffer), "-");
                        else
                            snprintf(buffer, sizeof(buffer), "%d", datasource->months[R].max_temp);
                        break;
                }
                fl_font(FL_HELVETICA, 12);
                fl_color(FL_BLACK);
            }
            
            if (buffer[0] != '\0')
            {
                fl_draw(buffer, X, Y, W, H, FL_ALIGN_CENTER);
            }
            
            fl_pop_clip();
            break;
        }
        
        default:
            break;
    }
};

void StatsTable::clear_data()
{
    datasource = nullptr;
    rows(0);
    redraw();
};

void StatsTable::resize(int X, int Y, int W, int H)
{
    Fl_Table::resize(X, Y, W, H); 
    this->recalc_dimensions();
    this->table_resized(); 
};

///////////////////////////////////////////////////////////////
DataTable::DataTable(int X, int Y, int W, int H, const char *L) 
    : Fl_Table(X, Y, W, H, L) 
{ 
    rows(0);
    cols(7);
    col_header(1);      
    col_header_height(25); 
    type(Fl_Scroll::BOTH);  
    scrollbar_size(16);
    row_height_all(25);  
    col_width_all(80);

    begin();
    box(FL_THIN_DOWN_FRAME);
    color(FL_WHITE);
    selection_color(fl_rgb_color(51, 153, 255));
    end();
    
    init_sizes();
};

void DataTable::set_data(IStorage_t* data)
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
    this->init_sizes(); 
    this->recalc_dimensions(); 
    this->redraw();
};

void DataTable::resize(int X, int Y, int W, int H)
{
    Fl_Table::resize(X, Y, W, H); 
    this->recalc_dimensions();
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
        {
            if (W <= 0 || H <= 0) break;
            
            fl_push_clip(X, Y, W, H);
            fl_draw_box(FL_THIN_UP_BOX, X, Y, W, H, FL_BACKGROUND_COLOR);
            fl_color(FL_BLACK);
            
            const char* headers[] = {"№", "Год", "Месяц", "День", 
                                     "Час", "Мин.", "T (°C)"};
            if (C >= 0 && C < 7)
            {
                fl_draw(headers[C], X, Y, W, H, FL_ALIGN_CENTER);
            }
            
            fl_pop_clip();
            break;
        }

        case CONTEXT_CELL: 
        { 
            if (R < 0 || C < 0 || R >= rows() || C >= cols() || W <= 0 || H <= 0) 
            {
                break;
            }
            
            fl_push_clip(X, Y, W, H);
            fl_color(FL_WHITE);
            fl_rectf(X, Y, W, H);
            fl_color(fl_rgb_color(220, 220, 220));
            fl_line_style(FL_SOLID, 1);
            fl_rect(X, Y, W, H);
            
            if (datasource && R >= 0 && R < (int)datasource->size(datasource)) 
            {
                struct TemperatureStats* tarr = (struct TemperatureStats*)datasource->raw_data(datasource);
                
                if (tarr) 
                {
                    buffer[0] = '\0';
                    
                    switch(C) {
                        case 0: snprintf(buffer, sizeof(buffer), "%d", R + 1); break;
                        case 1: snprintf(buffer, sizeof(buffer), "%u", tarr[R].year); break;
                        case 2: snprintf(buffer, sizeof(buffer), "%u", tarr[R].month); break;
                        case 3: snprintf(buffer, sizeof(buffer), "%u", tarr[R].day); break;
                        case 4: snprintf(buffer, sizeof(buffer), "%u", tarr[R].hours); break;
                        case 5: snprintf(buffer, sizeof(buffer), "%u", tarr[R].minutes); break;
                        case 6: snprintf(buffer, sizeof(buffer), "%d", tarr[R].temperature); break;
                    }
                    
                    fl_color(FL_BLACK);
                    fl_draw(buffer, X + 4, Y, W - 8, H, FL_ALIGN_CENTER);
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
        browse_errs->box(FL_FLAT_BOX);
        flex_errors->box(FL_FLAT_BOX);
        flex_errors->end();
        flex_errors->hide();
    }

    {
        flex_table_container = new Fl_Flex(0, 0, 100, 100);
        flex_table_container->type(Fl_Flex::VERTICAL);
        flex_table_container->margin(2, 2, 2, 2); 
        table_parse = new DataTable(0, 0, 100, 100);
        table_parse->hide();
        table_statistics = new StatsTable(0, 0, 100, 100);
        table_statistics->hide();
        flex_table_container->resizable(table_parse);
        flex_table_container->resizable(table_statistics);
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
    window->btn_report->callback(clb_report, this);
}

void View::update(const std::any &data, InterfacesApp::DataType datatype)
{
    switch (datatype)
    {
    case InterfacesApp::DataType::FullRowsList:
        try 
        {
            current_dataset = std::any_cast<IStorage_t*>(data);
            std::string msgtotalrow = "@C4Валидных строк: " + std::to_string(current_dataset->size(current_dataset));
            window->browse_errs->add(msgtotalrow.c_str());
            window->table_parse->redraw();
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
            std::string msgerrsize = "Ошибок: " + std::to_string(size);
            window->browse_errs->add(msgerrsize.c_str());
            if (size > 0)
            {
                window->flex_errors->show();
                window->main_flex->layout();
                for (size_t i = 0; i < size; ++i)
                {
                    struct ErrorParse *item_err = (struct ErrorParse *)errors->get(errors, i);
                    std::string msg = "@C1" + std::string(item_err->error_message);
                    window->browse_errs->add(msg.c_str());
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
        try 
        {
            datasource = std::any_cast<Statistics*>(data);
            window->table_statistics->redraw();
        }
        catch (const std::bad_any_cast& e) 
        {
            datasource = nullptr;
            printf("Type bad error\n");
        }
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

void View::handle_print_report()
{
    if (on_start_stats)
    {
        on_start_stats();
    }
    if (window && datasource)
    {
        window->table_statistics->set_data(datasource);
        window->table_parse->hide();
        window->table_statistics->show();
        window->main_flex->layout(); 
        
        window->table_statistics->redraw();
    }
    else
    {
        printf("EEERRRR view.cpp 503\n");
        window->table_statistics->set_data(nullptr);
        fl_alert("Нет данных для отображения! Сначала выберите файл и запустите парсинг.");
    }
};

void View::handle_print_all()
{
    if (window && current_dataset && current_dataset->size(current_dataset) > 0)
    {
        window->table_parse->set_data(current_dataset);
        window->table_statistics->hide();
        window->table_parse->show();
        window->main_flex->layout(); 
        static_cast<Fl_Widget*>(window->table_parse)->resize(
            window->table_parse->x(), 
            window->table_parse->y(), 
            window->table_parse->w(), 
            window->table_parse->h()
        );
        window->table_parse->redraw();
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
    {   
        w->show();
        f->layout();
        if (window->table_parse && window->table_parse->visible())
        {
            window->table_parse->resize(
                window->table_parse->x(), window->table_parse->y(),
                window->table_parse->w(), window->table_parse->h()
            );
        }
        window->redraw();
    }
    Fl::flush();
}

void View::hide_widget(Fl_Widget* w, Fl_Flex* f)
{
    if (window)
    {   
        w->hide();
        f->layout();
        if (window->table_parse && window->table_parse->visible())
        {
            window->table_parse->resize(
                window->table_parse->x(), window->table_parse->y(),
                window->table_parse->w(), window->table_parse->h()
            );
        }
        window->redraw();
    }
    Fl::flush();
};

void View::clear_table_parse()
{
    if (window && window->table_parse)
    {
        window->table_parse->set_data(nullptr);
        window->table_parse->hide();
        window->flex_table_container->layout();
        window->main_flex->layout();
        window->redraw();
        current_dataset = nullptr;
    }
};

void View::clear_table_stats()
{
    if (window && window->table_statistics)
    {
        window->table_statistics->set_data(nullptr);
        window->table_statistics->hide();
        window->flex_table_container->layout();
        window->main_flex->layout();
        window->redraw();
        datasource = nullptr;
    }
};

} // namespace ViewApp
