#include <stdio.h>
#include <stdlib.h>
#include "model.hpp"
#include "view.hpp"
#include "controller.hpp"

#ifdef _WIN32
#include <windows.h>
#include <FL/x.H>
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

int main (int argc, char **argv)
{
    Fl::scheme("gtk+");
    Fl::set_font(FL_FREE_FONT + 2, "Liberation Sans");
    auto* window = new ViewApp::AppWindow(800, 640, "appreport Курсовая.");
    auto* view = new ViewApp::View(window);
    auto* model = new ModelApp::ModelParse();
    auto* controller = new ControllerApp::AppController(view, model);
    
    window->show(argc, argv);
    Fl::check();

    #ifdef _WIN32
        seticon(window);
    #endif
    int exit_code = Fl::run();

    delete controller;
    delete model;
    delete view;
    delete window;

    return exit_code;
}

// git pull origin main