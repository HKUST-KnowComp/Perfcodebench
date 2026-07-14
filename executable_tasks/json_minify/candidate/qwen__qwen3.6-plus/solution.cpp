#include "interface.h"
#include <simdjson.h>

std::string minify_json(const std::string& json_input) {
    std::string out;
    simdjson::minify(json_input, out);
    return out;
}