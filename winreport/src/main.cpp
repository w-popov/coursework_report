#include <stdio.h>
#include <stdlib.h>
#include "model.hpp"
#include "view.hpp"
#include "controller.hpp"

int main (int argc, char **argv)
{
    auto* window = new ViewApp::AppWindow(650, 550, "appreport Курсовая.");
    auto* model = new ModelApp::ModelParse();
    auto* controller = new ControllerApp::AppController(window, model);
    
    window->show(argc, argv);
    int exit_code = Fl::run();

    delete controller;
    delete model;
    delete window;

    return exit_code;
}