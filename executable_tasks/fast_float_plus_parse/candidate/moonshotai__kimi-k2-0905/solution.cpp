#include "interface.h"
#include <fast_float/fast_float.h>
#include <string_view>

double parse_sum(const std::string& input) {
    double sum = 0.0;
    std::string_view sv(input);
    while (!sv.empty()) {
        double value;
        auto [ptr, ec] = fast_float::from_chars(sv.data(), sv.data() + sv.size(), value);
        if (ec != std::errc()) break;
        sum += value;
        sv.remove_prefix(ptr - sv.data());
        if (!sv.empty() && sv.front() == '\n') {
            sv.remove_prefix(1);
        }
    }
    return sum;
}