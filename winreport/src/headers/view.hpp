#pragma once

#include <FL/Fl_Progress.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Double_Window.H>
#include <FL/Fl_Flex.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Browser.H>
#include <FL/Fl_Table.H>
#include <FL/fl_draw.H>
#include <functional>
#include <any>
#include <cstdint> 
#include <string>
#include "interfaces.hpp" 
#include "parser_csv.h"
#include "temp_api.h"

namespace ViewApp
{


class DataTable : public Fl_Table 
{
private:
    IStorage_t* datasource = nullptr;
    
    public:
    DataTable(int X, int Y, int W, int H, const char *L = 0);
    
    // Метод для обновления данных из Observer
    void set_data(IStorage_t* data);
    void resize(int X, int Y, int W, int H) override;
    
protected:
    // Главный метод отрисовки ячеек
    void draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H) override; 
};


class StatsTable : public Fl_Table 
{
    private:
      struct Statistics *datasource = nullptr; 
      std::vector<std::string> column_headers;
        
    public:
      int month = 0;
      bool is_total = true;

      StatsTable(int X, int Y, int W, int H, const char *L = 0);
      void set_data(struct Statistics *stats); 
      void clear_data();
      void resize(int X, int Y, int W, int H) override;
      bool is_valid_month();
      void show_month_dialog();
    
    protected:
      void draw_cell(TableContext context, int R, int C, int X, int Y, int W, int H) override;
};


class AppWindow : public Fl_Double_Window
{
  public:
    Fl_Flex *main_flex;
    Fl_Flex *flex_top_buttons;
    Fl_Button *btn_open_file;
    Fl_Button *btn_parse_csv;
    Fl_Button *btn_report;
    Fl_Button *btn_report_month;
    Fl_Button *btn_all;
    Fl_Button *btn_save;
    Fl_Button *btn_exit;

    Fl_Flex *flex_errors;
    Fl_Browser *browse_errs;

    Fl_Flex *flex_table_container;
    DataTable *table_parse;
    StatsTable *table_statistics;

    Fl_Flex *flex_bottom_status;
    Fl_Box *lbl_file_name;
    Fl_Progress *progress_bar;

    void init_layout ();
    
    AppWindow(int width, int height, const char *title = nullptr);
};


class View : public InterfacesApp::Observer
{
    private:
      IStorage_t *current_dataset = nullptr;
      struct Statistics *datasource = nullptr;
    public:
      /* Эти функции вызовет контроллер для связи с моделью */
      std::function<void(const char* path)> on_file_selected = nullptr;
      std::function<void()> on_start_parsing = nullptr;
      std::function<void()> on_start_stats = nullptr;

      /* Вызывают колбэки кнопок */
      void handle_open_file ();
      void handle_parse_csv ();
      void handle_print_all();
      void handle_print_report();
      void handle_print_report_month();

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

      // колбэк кнопки Данные
      static void clb_all (Fl_Widget *w, void *data)
      {
          (void)w;
          static_cast<View *>(data)->handle_print_all();
      }

      // колбэк кнопки Отчет
      static void clb_report (Fl_Widget *w, void *data)
      {
          (void)w;
          static_cast<View *>(data)->handle_print_report();
      }

      // колбэк кнопки Отчет за месяц
      static void clb_report_month (Fl_Widget *w, void *data)
      {
          (void)w;
          static_cast<View *>(data)->handle_print_report_month();
      }

      AppWindow *window = nullptr; 

      virtual ~View() = default;
      explicit View(Fl_Double_Window *w);

      void update(const std::any& data, InterfacesApp::DataType datatype) override; 

      void show_widget(Fl_Widget*, Fl_Flex*);
      void hide_widget(Fl_Widget*, Fl_Flex*);
      void clear_table_parse();
      void clear_table_stats();

};

} // namespace ViewApp
