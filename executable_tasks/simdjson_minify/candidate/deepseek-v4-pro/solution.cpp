#include "interface.h"

#include <simdjson.h>

std::string minify_json(const std::string& json_input) {
    return simdjson::minify(json_input).value();
}