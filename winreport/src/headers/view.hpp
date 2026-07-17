#pragma once

#include <stdint.h>
#include <FL/Fl_Progress.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Table.H>

namespace ViewApp
{

class AppWindow : public Fl_Double_Window
{
  private:
    Fl_Flex *main_flex;
    Fl_Flex *flex_top_buttons;
    Fl_Button *btn_open_file;
    Fl_Button *btn_parse_csv;

    Fl_Flex *flex_errors;
    Fl_Browser *browse_errs;

    Fl_Flex *flex_table_container;
    Fl_Table *table_parse;

    Fl_Flex *flex_bottom_status;
    Fl_Box *lbl_file_name;
    Fl_Progress *progress_bar;

    void init_layout ();

    void handle_open_file ();
    void handle_parse_csv ();

    static void cb_open_file (Fl_Widget *w, void *data)
    {
        (void)w;
        static_cast<AppWindow *>(data)->handle_open_file();
    }

    static void cb_parse_csv (Fl_Widget *w, void *data)
    {
        (void)w;
        static_cast<AppWindow *>(data)->handle_parse_csv();
    }

  public:
    virtual ~AppWindow() = default;
    AppWindow(int width, int height, const char *title = nullptr);

    void (*on_file_selected)(const char *path, void *context) = nullptr;
    void (*on_start_parsing)(void *context) = nullptr;
    // Объект контекста (модель или вью)
    void *callback_context = nullptr;

    void set_status_file (const char *name);
    void update_progress_bar_value (int64_t current, int64_t total);
};

} // namespace ViewApp
