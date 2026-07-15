#pragma once

#include <any>
#include <vector>
#include <algorithm>

namespace InterfacesApp
{

class AppError;

/**
 * Наблюдатель
 */
class Observer
{
  public:
    virtual ~Observer() = default;
    virtual void on_model_changed (
        const std::any &data, const AppError *error = nullptr
    ) = 0;
};

/** 
 * Базовый класс для источников данных (Model)
 */
class Observable
{
  private:
    std::vector<Observer *> observers;

  public:
    virtual ~Observable() = default;
    void add_observer (Observer *o);
    void remove_observer (Observer *o);

  protected:
    void notify_observers 
    (const std::any &data, const AppError* error = nullptr);
};

} // namespace InterfacesApp
