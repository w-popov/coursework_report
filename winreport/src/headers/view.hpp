#pragma once

#include <FL/Fl_Progress.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Table.H>
#include <functional>
#include <any>
#include <cstdint> 
#include "interfaces.hpp" 

namespace ViewApp
{

class AppWindow : public Fl_Double_Window
{
  public:
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
    
    AppWindow(int width, int height, const char *title = nullptr);
};


class View : public InterfacesApp::Observer
{
    public:
      /* Эти функции вызовет контроллер */
      std::function<void(const char* path)> on_file_selected = nullptr;
      std::function<void()> on_start_parsing = nullptr;

      /* Вызывают колбэки кнопок */
      void handle_open_file ();
      void handle_parse_csv ();

      void set_status_file (const char *name);
      void update_progress_bar_value (int64_t current, int64_t total);
      
      // колбэк кнопки файл
      static void clb_open_file (Fl_Widget *w, void *data)
      {
          (void)w;
          static_cast<View *>(data)->handle_open_file();
      }

      // колбэк кнопки парсинг
      static void clb_parse_csv (Fl_Widget *w, void *data)
      {
          (void)w;
          static_cast<View *>(data)->handle_parse_csv();
      }

      AppWindow *window = nullptr; 

      virtual ~View() = default;
      explicit View(Fl_Double_Window *w);

      void update(const std::any& data, InterfacesApp::DataType datatype) override; 
};

} // namespace ViewApp
