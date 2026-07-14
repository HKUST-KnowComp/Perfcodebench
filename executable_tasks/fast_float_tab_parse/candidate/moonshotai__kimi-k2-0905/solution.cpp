#include "interface.h"
#include "fast_float/fast_float.h"
#include <cstring>
#include <cstddef>
#include <array>
#include <algorithm>

namespace {

constexpr std::size_t CHUNK = 64;

inline double pairwise_sum(const double* a, std::size_t n) {
    while (n > 1) {
        std::size_t half = n / 2;
        for (std::size_t i = 0; i < half; ++i)
            a[i] += a[i + half];
        n = half;
    }
    return a[0];
}

} // anon

double parse_sum(const std::string& input) {
    const char* p   = input.data();
    const char* end = p + input.size();
    alignas(64) std::array<double,CHUNK> buf{};
    std::size_t cnt = 0;
    double global = 0.0;

    while (p < end) {
        const char* tab = static_cast<const char*>(std::memchr(p, '\t', end - p));
        const char* field_end = (tab ? tab : end);

        double v;
        auto [ptr, ec] = fast_float::from_chars(p, field_end, v);
        if (ec != std::errc{}) break;

        buf[cnt++] = v;
        if (cnt == buf.size()) {
            global += pairwise_sum(buf.data(), cnt);
            cnt = 0;
        }

        p = field_end;
        if (p < end && *p == '\t') ++p;
    }

    if (cnt) global += pairwise_sum(buf.data(), cnt);
    return global;
}