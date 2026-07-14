#include "interface.h"

#include <string>

std::string minify_json(const std::string& json_input) {
    std::string result;
    result.reserve(json_input.size());
    
    bool in_string = false;
    bool escaped = false;
    
    for (size_t i = 0; i < json_input.size(); ++i) {
        char c = json_input[i];
        
        if (escaped) {
            result += c;
            escaped = false;
            continue;
        }
        
        if (c == '\\' && in_string) {
            result += c;
            escaped = true;
            continue;
        }
        
        if (c == '"') {
            in_string = !in_string;
            result += c;
            continue;
        }
        
        if (in_string) {
            result += c;
            continue;
        }
        
        // Outside string: skip whitespace
        if (c == ' ' || c == '\t' || c == '\n' || c == '\r') {
            continue;
        }
        
        result += c;
    }
    
    return result;
}