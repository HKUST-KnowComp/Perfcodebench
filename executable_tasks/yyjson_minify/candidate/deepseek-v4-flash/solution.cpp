#include "interface.h"
#include <yyjson.h>
#include <cstdlib>
#include <cstddef>

std::string minify_json(const std::string& json_input) {
    yyjson_doc *doc = yyjson_read(json_input.c_str(), json_input.size(), 0);
    if (!doc) {
        return "";
    }
    size_t len;
    char *json_str = yyjson_write(doc, 0, &len);
    std::string result(json_str, len);
    free(json_str);
    yyjson_doc_free(doc);
    return result;
}
