#include "interface.h"
#include <simdjson.h>
#include <string>

std::string minify_json(const std::string& json_input) {
    if (json_input.empty()) return "";

    std::string result;
    result.reserve(json_input.size());

    bool in_string = false;
    bool escaped = false;

    for (size_t i = 0; i < json_input.size(); ++i) {
        char c = json_input[i];

        if (in_string) {
            result += c;
            if (escaped) {
                escaped = false;
            } else if (c == '\\') {
                escaped = true;
            } else if (c == '"') {
                in_string = false;
            }
        } else {
            if (c == '"') {
                in_string = true;
                result += c;
            } else if (!std::isspace(static_cast<unsigned char>(c))) {
                result += c;
            }
        }
    }

    return result;
}