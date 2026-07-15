
#pragma once

#include <exception>
#include <string>

namespace ErrorsApp
{
    /**
     * Базовый класс исключений приложения
     */
    class AppError : public std::exception 
    {
    protected:
        std::string err_msg;

    public:
        explicit AppError(std::string error) : err_msg(std::move(error)) {}

        const char* what() const noexcept override 
        { 
            return err_msg.c_str();
        }
    };
}
