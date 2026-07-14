#include "interface.h"

#include <yyjson.h>

std::string minify_json(const std::string& json_input) {
    yyjson_doc *doc = yyjson_read(json_input.data(), json_input.size(), 0);
    if (!doc) {
        return "";
    }
    
    size_t len = 0;
    char *json_str = yyjson_write(doc, 0, &len);
    yyjson_doc_free(doc);
    
    if (!json_str) {
        return "";
    }
    
    std::string result(json_str, len);
    free(json_str);
    
    return result;
}