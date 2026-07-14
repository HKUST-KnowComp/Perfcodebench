#include "interface.h"

#include <simdjson.h>

std::string minify_json(const std::string& json_input) {
    std::string output;
    output.resize(json_input.size());
    size_t output_len = 0;
    auto error = simdjson::minify(json_input.data(), json_input.size(), output.data(), output_len);
    if (error != simdjson::SUCCESS) {
        output.clear();
        return output;
    }
    output.resize(output_len);
    return output;
}