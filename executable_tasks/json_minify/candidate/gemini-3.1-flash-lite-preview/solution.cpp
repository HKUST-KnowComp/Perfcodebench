#include "interface.h"
#include "simdjson.h"

std::string minify_json(const std::string& json_input) {
    simdjson::ondemand::parser parser;
    simdjson::padded_string padded(json_input);
    auto doc = parser.iterate(padded);
    
    // Using simdjson's ability to serialize the document back to a string.
    // Since we need the minified version, we can use the default dump behavior.
    return simdjson::to_json_string(doc);
}