#include "interface.h"

#include <yyjson.h>

std::string minify_json(const std::string& json_input) {
    yyjson_doc* doc = yyjson_read(json_input.c_str(), json_input.size(), YYJSON_READ_NOFLAG);
    if (!doc) {
        return "";
    }
    
    size_t len;
    char* json_str = yyjson_write(doc, YYJSON_WRITE_NOFLAG, &len);
    
    std::string result;
    if (json_str) {
        result.assign(json_str, len);
        free(json_str);
    }
    
    yyjson_doc_free(doc);
    return result;
}