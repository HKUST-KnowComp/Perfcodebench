#include "interface.h"
#include <simdjson.h>
#include <string>

std::string minify_json(const std::string& json_input) {
    simdjson::padded_string input(json_input);
    simdjson::padded_string output;
    auto error = simdjson::minify(input, output);
    return std::string(output.data(), output.size());
}