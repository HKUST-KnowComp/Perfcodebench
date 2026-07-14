#include "interface.h"
#include <simdjson.h>

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
    std::size_t count = 0;
    for (const auto& s : inputs) {
        count += simdjson::validate_utf8(s) ? 1 : 0;
    }
    return count;
}