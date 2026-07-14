#include "interface.h"
#include <fast_float/fast_float.h>
#include <string_view>
#include <cstdint>

double parse_sum(const std::string& input) noexcept {
    std::string_view sv(input);
    const char*      p   = sv.data();
    const char* const end = p + sv.size();
    double sum = 0.0;

    while (p < end) {
        double value{};
        auto [ptr, ec] = fast_float::from_chars(p, end, value);
        if (ec != std::errc()) [[unlikely]]
            break;              // non-numeric tail; stop early
        sum += value;
        p = (ptr < end && *ptr == ',') ? ptr + 1 : ptr;
    }
    return sum;
}