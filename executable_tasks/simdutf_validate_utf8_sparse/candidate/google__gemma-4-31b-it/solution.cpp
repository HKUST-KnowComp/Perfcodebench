#include "interface.h"
#include "simdutf.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
    std::size_t count = 0;
    for (const auto& s : inputs) {
        // simdutf::validate_utf8 returns true if the input is valid UTF-8.
        // It is highly optimized using SIMD instructions (AVX2, NEON, etc.)
        // and is significantly faster than scalar validation for long strings.
        if (simdutf::validate_utf8(reinterpret_cast<const char*>(s.data()), s.size())) {
            count++;
        }
    }
    return count;
}