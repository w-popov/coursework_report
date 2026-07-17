#pragma once

#include "interfaces.hpp"

namespace ViewApp { class View; }
namespace ModelApp { class ModelParse; }

namespace ControllerApp
{

class AppController
{
private:
    ViewApp::View* view;
    ModelApp::ModelParse* model;

public:
    ~AppController();
    AppController(ViewApp::View*, ModelApp::ModelParse*);

    // Установить прогрессбар
    static void progress_bridge(void* self, int64_t current, int64_t total);
    
};

} // namespace ControllerApp
