#include "view.hpp"
#include <FL/Fl.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <filesystem>
#include <cstdio>

namespace ViewApp
{

AppWindow::AppWindow(int width, int height, const char *title)
    : Fl_Double_Window(width, height, title)
{
    this->box(FL_FLAT_BOX);
    this->color(FL_BACKGROUND_COLOR);
    this->selection_color(FL_BACKGROUND_COLOR);
    this->labeltype(FL_NO_LABEL);
    this->align(FL_ALIGN_TOP);

    init_layout();

    btn_open_file->callback(cb_open_file, this);
    btn_parse_csv->callback(cb_parse_csv, this);
}

void AppWindow::init_layout()
{
    main_flex = new Fl_Flex(0, 0, this->w(), this->h());
    main_flex->margin(10, 10, 10, 10);
    main_flex->gap(10);

    {
        flex_top_buttons = new Fl_Flex(0, 0, 0, 40);
        flex_top_buttons->type(Fl_Flex::HORIZONTAL);

        btn_open_file = new Fl_Button(0, 0, 80, 40, "Файл");
        btn_open_file->labelsize(12);

        btn_parse_csv = new Fl_Button(0, 0, 80, 40, "Парсинг");
        btn_parse_csv->labelsize(12);

        flex_top_buttons->gap(10);
        flex_top_buttons->fixed(btn_open_file, 80);
        flex_top_buttons->fixed(btn_parse_csv, 80);
        flex_top_buttons->end();
    }

    {
        flex_errors = new Fl_Flex(0, 0, 0, 100);
        browse_errs = new Fl_Browser(0, 0, 0, 100);
        flex_errors->end();
    }

    {
        flex_table_container = new Fl_Flex(0, 0, 0, 0);
        table_parse = new Fl_Table(0, 0, 0, 0);
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
        progress_bar->selection_color(FL_BLUE);

        flex_bottom_status->end();
    }

    main_flex->fixed(flex_top_buttons, 40);
    main_flex->fixed(flex_errors, 100);
    main_flex->fixed(flex_bottom_status, 45);
    main_flex->end();

    this->resizable(main_flex);
    this->end();
}

void AppWindow::handle_open_file()
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
            on_file_selected(path, callback_context);
        }
    }
}

void AppWindow::handle_parse_csv()
{
    if (on_start_parsing)
    {
        on_start_parsing(callback_context);
    }
}

void AppWindow::set_status_file(const char *name)
{
    if (lbl_file_name)
    {
        lbl_file_name->label(name);
        lbl_file_name->redraw();
    }
}

void AppWindow::update_progress_bar_value(int64_t current, int64_t total)
{
    if (!progress_bar || total <= 0)
    {
        return;
    }

    float percentage =
        (static_cast<float>(current) * 100.0f) / static_cast<float>(total);
    if (percentage > 100.0f)
    {
        percentage = 100.0f;
    }

    progress_bar->value(percentage);

    char pct_text[32];
    std::snprintf(pct_text, sizeof(pct_text), "Парсинг: %.1f%%", percentage);
    progress_bar->copy_label(pct_text);

    progress_bar->redraw();
    Fl::check();
}

} // namespace ViewApp
