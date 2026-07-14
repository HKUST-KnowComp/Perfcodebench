#include "interface.h"
#include <string>

std::string minify_json(const std::string& json_input) {
    std::string result;
    result.reserve(json_input.size());
    bool in_string = false;

    for (size_t i = 0; i < json_input.size(); ++i) {
        char c = json_input[i];
        if (in_string) {
            result.push_back(c);
            if (c == '\\') {
                if (i + 1 < json_input.size()) {
                    result.push_back(json_input[++i]);
                }
            } else if (c == '"') {
                in_string = false;
            }
        } else {
            if (c == '"') {
                in_string = true;
                result.push_back(c);
            } else if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
                // Skip whitespace
                continue;
            } else {
                result.push_back(c);
            }
        }
    }
    return result;
}