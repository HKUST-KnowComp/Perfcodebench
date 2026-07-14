#include "interface.h"
#include "simdjson.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
    std::size_t count = 0;
    for (const auto& s : inputs) {
        simdjson::error_code err = simdjson::validate_utf8(
            reinterpret_cast<const uint8_t*>(s.data()), s.size());
        if (err == simdjson::SUCCESS) {
            ++count;
        }
    }
    return count;
}