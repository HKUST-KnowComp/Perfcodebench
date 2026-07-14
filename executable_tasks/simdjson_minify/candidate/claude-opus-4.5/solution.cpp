#include "interface.h"

#include <simdjson.h>

std::string minify_json(const std::string& json_input) {
    std::string output;
    output.resize(json_input.size());
    size_t new_length{};
    auto error = simdjson::minify(json_input.data(), json_input.size(), output.data(), new_length);
    if (error != simdjson::SUCCESS) {
        output.clear();
        return output;
    }
    output.resize(new_length);
    return output;
}