#include "interface.h"
#include "simdjson.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
    std::size_t count = 0;
    for (const auto& s : inputs) {
        // simdjson::utf8::validate_utf8 returns a result indicating if the 
        // provided buffer is valid UTF-8. It is highly optimized using SIMD.
        if (simdjson::utf8::validate_utf8(reinterpret_cast<const uint8_t*>(s.data()), s.size())) {
            count++;
        }
    }
    return count;
}