#include "interface.h"
#include <fast_float/fast_float.h>
#include <string_view>

double parse_sum(const std::string& input) {
    std::string_view sv(input);
    double sum = 0.0;
    while (!sv.empty()) {
        double value;
        auto [ptr, ec] = fast_float::from_chars(sv.data(), sv.data() + sv.size(), value);
        if (ec != std::errc()) break;
        sum += value;
        size_t consumed = ptr - sv.data();
        if (consumed < sv.size() && sv[consumed] == ':') ++consumed;
        sv.remove_prefix(consumed);
    }
    return sum;
}