#include "model.hpp"
#include "validate.h"
#include "temp_api.h"
#include <cstring>

namespace ModelApp
{
  
//////////////////////// class ModelParse ////////////////////////
ModelParse::~ModelParse()
{
    model_free();
};

void ModelParse::model_init()
{
    array = svector_init((struct IStorage_t *)&array_vec, sizeof(struct TemperatureStats), 0);
    errors_array = svector_init((struct IStorage_t *)&err_array_vec, sizeof(struct ErrorParse), 0);
    context = new struct ContextParser;
    parse_source = new struct ParseSource;
    context->csv.current_column = 0;
    context->csv.current_row = 0;
    context->csv.delimiter = ";";
    context->csv.length_field = 0;
    context->csv.nums_field = 5;
    context->clbs.self = nullptr;
    context->clbs.clb_progress = nullptr;
    context->clbs.clb_write_to_arr = write_to_array;
    context->array = array;
    context->errors_parse = errors_array;
    parse_source->get_char = get_char_from_file;
    parse_source->get_pos = get_pos_from_file;
};

void ModelParse::model_free()
{
    if (array)
    {
        array->free(array);
        array = nullptr;
    }

    if (errors_array)
    {
        errors_array->free(errors_array);
        errors_array = nullptr;
    }

    if (parse_source)
    {
        delete parse_source;
        parse_source = nullptr;
    }

    if (context)
    {
        delete context;
        context = nullptr;
    }
};

void ModelParse::storages_clear()
{
    if (array)
    {
        array->free(array);
        array = nullptr;
    }

    if (errors_array)
    {
        errors_array->free(errors_array);
        errors_array = nullptr;
    }

    array = svector_init((struct IStorage_t *)&array_vec, sizeof(struct TemperatureStats), 0);
    errors_array = svector_init((struct IStorage_t *)&err_array_vec, sizeof(struct ErrorParse), 0);
    if (context)
    {
        context->csv.current_column = 0;
        context->csv.current_row = 0;
        context->csv.length_field = 0;
        memset(context->csv.buffer, '\0', MAX_FIELD_SIZE);
    }
};

void ModelParse::parsing(const char* filename)
{
    storages_clear();
    
    int64_t filesize = -1;
    FILE *file = open_file(filename, &filesize);
    if (!file) {
        notify(std::any{}, InterfacesApp::DataType::ErrorOpenFile);
        return; 
    }

    context->file_size = filesize;
    parse_source->stream = file;
        
    ContextParser *c = parse_csv(context, parse_source);
    fclose(file);
    parse_source->stream = nullptr;
    if (!c)
    {
        notify(std::any{}, InterfacesApp::DataType::ErrorNullParse);
        return;
    }
    notify(errors_array, InterfacesApp::DataType::ErrorsParse);
    notify(array, InterfacesApp::DataType::FullRowsList);

};


ModelParse::ModelParse()
{
    model_init();
};

}; // namespace ModelApp
