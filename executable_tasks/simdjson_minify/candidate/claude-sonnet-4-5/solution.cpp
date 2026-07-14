#include "interface.h"

#include <simdjson.h>

std::string minify_json(const std::string& json_input) {
    static thread_local simdjson::ondemand::parser parser;
    
    std::string result;
    result.reserve(json_input.size());
    
    simdjson::padded_string padded(json_input);
    auto doc = parser.iterate(padded);
    
    result = simdjson::minify(padded);
    
    return result;
}