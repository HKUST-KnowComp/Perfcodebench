#include "interface.h"
#include <fast_float/fast_float.h>
#include <string_view>

double parse_sum(const std::string& input) {
    std::string_view sv(input);
    double sum = 0.0;
    while (!sv.empty()) {
        size_t nl = sv.find('\n');
        std::string_view line = sv.substr(0, nl);
        double value;
        auto [ptr, ec] = fast_float::from_chars(line.data(), line.data() + line.size(), value);
        if (ec == std::errc()) {
            sum += value;
        }
        sv = (nl == std::string_view::npos) ? std::string_view() : sv.substr(nl + 1);
    }
    return sum;
}