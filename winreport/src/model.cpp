#include "model.hpp"
#include "validate.h"
#include "temp_api.h"

namespace ModelApp
{
  
//////////////////////// class ModelParse ////////////////////////
ModelApp::ModelParse::~ModelParse()
{
    if (array)
        array->free(array);

    if (errors_array)
        errors_array->free(errors_array);

    if (parse_source)
        delete parse_source;

    if (context)
        delete context;
};


void ModelApp::ModelParse::parsing(const char* filename)
{
    int64_t filesize = -1;
    FILE *file = open_file(filename, &filesize);
    if (!file) {
        notify(std::any{}, InterfacesApp::DataType::ErrorOpenFile);
        return; 
    }

    context->file_size = filesize;
    parse_source->stream = file;
    
    ContextParser *c = parse_csv(context, parse_source);
    if (!c)
    {
        notify(std::any{}, InterfacesApp::DataType::ErrorNullParse);
    }
    
    fclose(file);
    parse_source->stream = nullptr;

    notify(errors_array, InterfacesApp::DataType::ErrorsParse);
};


ModelApp::ModelParse::ModelParse()
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
    context->clbs.clb_progress = nullptr;
    context->clbs.clb_write_to_arr = write_to_array;
    context->array = array;
    context->errors_parse = errors_array;
    parse_source->get_char = get_char_from_file;
    parse_source->get_pos = get_pos_from_file;
};

}; // namespace ModelApp
