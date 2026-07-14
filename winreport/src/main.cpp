#include <FL/Fl.H>
#include <FL/Fl_Window.H>
#include <FL/Fl_Button.H>
#include <FL/Fl_Box.H>
#include <FL/Fl_Scroll.H>
#include <FL/Fl_Native_File_Chooser.H>
#include <FL/fl_ask.H>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <sys/stat.h>
#include "temp_api.h"
#include "parser_csv.h"
#include "validate.h"

#ifdef _WIN32
#include <windows.h>
#include <FL/x.H>
void seticon (Fl_Window *win);
#endif

#ifdef _WIN32
void seticon (Fl_Window *win)
{
    HWND hwnd = fl_xid(win);
    if (hwnd)
    {
        HICON hIcon = LoadIcon(GetModuleHandle(NULL), "MAINICON");
        if (hIcon)
        {
            SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM)hIcon);
            SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM)hIcon);
        }
    }
}
#endif

Fl_Window *window = nullptr;
Fl_Box *report_box = nullptr;
Fl_Scroll *scroll = nullptr;

#define REPORT_BUFFER_SIZE 65536
char report_text[REPORT_BUFFER_SIZE] = "Загрузите CSV файл...\n";

int append_to_report (const char *format, ...)
{
    size_t current_len = strlen(report_text);
    if (current_len >= REPORT_BUFFER_SIZE - 1)
    {
        return 0;
    }

    va_list args;
    va_start(args, format);
    int written = vsnprintf(report_text + current_len,
                            REPORT_BUFFER_SIZE - current_len, format, args);
    va_end(args);
    return written;
}

void clear_report ()
{
    report_text[0] = '\0';
}

void update_report_box_size ()
{
    if (!report_box || !scroll)
    {
        return;
    }

    int lines = 1;
    for (const char *p = report_text; *p; p++)
    {
        if (*p == '\n')
        {
            lines++;
        }
    }

    int line_height = report_box->labelsize() + 4;
    int new_height = lines * line_height + 20;

    if (new_height < scroll->h())
    {
        new_height = scroll->h();
    }

    report_box->size(scroll->w() - 20, new_height);
}

// Callback для кнопки загрузки
void load_csv_cb (Fl_Widget *, void *)
{
    Fl_Native_File_Chooser chooser;
    chooser.title("Выберите CSV файл");
    chooser.type(Fl_Native_File_Chooser::BROWSE_FILE);
    chooser.filter("CSV Files\t*.csv\nAll Files\t*.*");

    if (chooser.show() == 0)
    {
        const char *filename = chooser.filename();
        clear_report();
        // размер файла
        struct stat st;
        long filesize = 0;
        if (stat(filename, &st) == 0)
        {
            filesize = st.st_size;
        }
        append_to_report("========================================\n");
        append_to_report("      ОТЧЁТ О ТЕМПЕРАТУРНЫХ ДАННЫХ\n");
        append_to_report("========================================\n");
        append_to_report("      Файл: %s\n", filename);
        append_to_report("      Размер: %ld байт\n", filesize);
        append_to_report("----------------------------------------\n\n");

        FILE *file = fopen(filename, "r");
        if (!file)
        {
            append_to_report("ОШИБКА: не удалось открыть файл!\n");
            report_box->label(report_text);
            update_report_box_size();
            window->redraw();
            return;
        }

        // Инициализация структур для парсинга
        struct SVector t_array_vec;
        struct SVector err_array_vec;

        struct IStorage_t *array =
            svector_init((struct IStorage_t *)&t_array_vec,
                         sizeof(struct TemperatureStats), 0);
        struct IStorage_t *errors_array = svector_init(
            (struct IStorage_t *)&err_array_vec, sizeof(struct ErrorParse), 0);

        if (!array || !errors_array)
        {
            append_to_report("ОШИБКА: не удалось создать хранилище!\n");
            fclose(file);
            report_box->label(report_text);
            update_report_box_size();
            window->redraw();
            return;
        }

        // Настройка контекста парсера
        struct ContextParser cntx;
        memset(&cntx, 0, sizeof(cntx));
        cntx.csv.delimiter = ";";
        cntx.csv.nums_field = 5;
        cntx.clbs.clb_write_to_arr = write_to_array;
        cntx.clbs.clb_progress = NULL;
        cntx.array = array;
        cntx.errors_parse = errors_array;
        cntx.file_size = filesize;

        struct ParseSource file_source = {.stream = file,
                                          .get_char = get_char_from_file,
                                          .get_pos = get_pos_from_file};

        // Вызов парсера CSV
        parse_csv(&cntx, &file_source);

        fclose(file);

        // Вывод результатов
        append_to_report("      Данные успешно загружены!\n");
        append_to_report("      Записей обработано: %zu\n", array->size(array));
        append_to_report("      Ошибок парсинга: %zu\n",
                         errors_array->size(errors_array));
        append_to_report("----------------------------------------\n\n");

        // Вывод данных
        struct TemperatureStats *tarr =
            (struct TemperatureStats *)array->raw_data(array);
        for (size_t i = 0; i < array->size(array); ++i)
        {
            append_to_report(
                "%6zu]  Year: %4u,  Month: %2u,  Day: %2u,  Hours: %2u,  "
                "Minutes: %2u,  Temperature: %2d\n",
                i + 1, tarr[i].year, tarr[i].month, tarr[i].day, tarr[i].hours,
                tarr[i].minutes, tarr[i].temperature);
        }

        // Очистка памяти
        array->free(array);
        errors_array->free(errors_array);

        // Обновление Fl_Box и размера
        report_box->label(report_text);
        update_report_box_size();
        scroll->redraw();
        window->redraw();
    }
}

// Callback для кнопки "О программе"
void about_cb (Fl_Widget *, void *)
{
    fl_message("Программа чтения CSV отчётов\n"
               "Курсовая работа\n"
               "гр. Д01-134 Попов В.Г.");
}

int main (int argc, char **argv)
{
    Fl::scheme("gtk+");

    window = new Fl_Window(800, 700, "Report Viewer v1.0");

    // Заголовок окна
    Fl_Box *title = new Fl_Box(20, 10, 760, 30, "Программа чтения CSV отчётов");
    title->labelfont(FL_BOLD);
    title->labelsize(16);

    scroll = new Fl_Scroll(20, 50, 760, 580);
    scroll->type(
        Fl_Scroll::VERTICAL_ALWAYS); // Всегда показывать полосу прокрутки
    scroll->color(FL_WHITE);
    scroll->box(FL_DOWN_BOX);

    report_box = new Fl_Box(0, 0, 740, 580, report_text);
    report_box->box(FL_FLAT_BOX);
    report_box->labelfont(FL_COURIER);
    report_box->labelsize(12);
    report_box->align(FL_ALIGN_TOP_LEFT | FL_ALIGN_INSIDE);
    report_box->color(FL_WHITE);
    report_box->labelcolor(FL_BLACK);

    scroll->end();

    Fl_Button *load_btn = new Fl_Button(250, 650, 120, 30, "Загрузить CSV");
    load_btn->callback(load_csv_cb);

    Fl_Button *about_btn = new Fl_Button(390, 650, 120, 30, "О программе");
    about_btn->callback(about_cb);

    Fl_Button *exit_btn = new Fl_Button(530, 650, 120, 30, "Выход");
    exit_btn->callback([] (Fl_Widget *, void *) { exit(0); });

    window->end();
    window->show(argc, argv);
    Fl::check();
#ifdef _WIN32
    seticon(win);
#endif

    return Fl::run();
}