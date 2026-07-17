#pragma once

#include "interfaces.hpp"

namespace ViewApp { class AppWindow; }
namespace ModelApp { class ModelParse; }

namespace ControllerApp
{

class AppController : public InterfacesApp::Observer
{
private:
    ViewApp::AppWindow* view;
    ModelApp::ModelParse* model;

public:
    AppController(ViewApp::AppWindow* window, ModelApp::ModelParse* model_parse);
    virtual ~AppController();

    // Реализация Observer
    void update(const std::any& data, InterfacesApp::DataType datatype) override;
};

} // namespace ControllerApp
