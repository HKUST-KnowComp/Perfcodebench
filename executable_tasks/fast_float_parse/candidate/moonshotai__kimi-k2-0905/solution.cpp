#include "interface.h"

#include <fast_float/fast_float.h>
#include <cstring>
#include <cstdint>

// ---- constants -----------------------------------------------------------
static constexpr uint64_t MAX_BATCH = 1ULL << 20; // ~1M numbers

// ---- helper: parse a range [first,last) into a double --------------------
static inline double parse_range(const char* first, const char* last)
{
    double v = 0.0;
    fast_float::from_chars(first, last, v);
    return v;
}

// ---- main routine --------------------------------------------------------
double parse_sum(const std::string& input)
{
    const char* p   = input.data();
    const char* end = p + input.size();

    // fast path: empty input
    if (p == end) return 0.0;

    // Use 64-bit accumulator to reduce rounding error, fold into double once.
    __int128 total128 = 0;
    uint64_t   count   = 0;

    while (p < end)
    {
        const char* line_end = static_cast<const char*>(
            std::memchr(p, '\n', end - p));
        if (!line_end) line_end = end;

        double v = parse_range(p, line_end);
        total128 += static_cast<int64_t>(v * (1LL << 30));
        ++count;

        p = (line_end < end && *line_end == '\n') ? line_end + 1 : line_end;

        // Flush to double periodically to avoid overflow in the 128-bit int
        if (count >= MAX_BATCH)
        {
            total128 = static_cast<__int128>(static_cast<double>(total128) / (1LL << 30));
            count = 0;
        }
    }
    return static_cast<double>(total128) / (1LL << 30);
}