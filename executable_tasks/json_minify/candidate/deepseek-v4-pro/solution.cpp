#include "interface.h"

#include <string>

std::string minify_json(const std::string& json_input) {
    std::string result;
    result.reserve(json_input.size());
    bool in_string = false;
    bool escape = false;
    for (char c : json_input) {
        if (in_string) {
            result.push_back(c);
            if (escape) {
                escape = false;
            } else if (c == '\\') {
                escape = true;
            } else if (c == '"') {
                in_string = false;
            }
        } else {
            if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
                continue;
            }
            result.push_back(c);
            if (c == '"') {
                in_string = true;
            }
        }
    }
    return result;
}
