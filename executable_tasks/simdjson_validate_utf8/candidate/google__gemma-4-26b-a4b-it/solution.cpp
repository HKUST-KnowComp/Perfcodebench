#include "interface.h"
#include "simdjson.h"

namespace {

// Helper to validate a single string using simdjson's optimized UTF-8 validation.
// simdjson::is_valid_utf8 is highly optimized using SIMD instructions.
inline bool is_valid_utf8_simd(const std::string& s) {
    // We use simdjson::padded_string to ensure the buffer has enough padding
    // for SIMD reads, though for a single string validation, we can also use
    // the direct validation API on the raw pointer if we handle bounds carefully.
    // However, the most robust way within the simdjson ecosystem is to use
    // the validation function provided by the library.
    return simdjson::is_valid_utf8(s.data(), s.size());
}

} // namespace

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
    std::size_t count = 0;
    for (const auto& s : inputs) {
        if (simdjson::is_valid_utf8(s.data(), s.size())) {
            count++;
        }
    }
    return count;
}