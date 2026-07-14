#include "interface.h"
#include <fast_float/fast_float.h>
#include <cstdint>
#include <cstring>

namespace {

// Parse one Fortran-style double in-place.
// Returns pointer past the last consumed character and the value.
inline const char* parse_double(const char* p, const char* end, double& value) noexcept {
    const char* start = p;
    // Skip leading whitespace
    while (p < end && (*p == ' ' || *p == '\t')) ++p;
    if (p == end) return start; // empty

    // Find first 'd' or 'D' in probable numeric span
    const char* d_pos = nullptr;
    const char* q = p;
    for (; q < end && *q != '\n'; ++q) {
        if ((*q == 'd' || *q == 'D') && !d_pos) d_pos = q;
    }
    const char* line_end = q;

    char patch = 0;
    if (d_pos) {
        patch = *d_pos;          // save original
        *const_cast<char*>(d_pos) = (patch == 'd' ? 'e' : 'E'); // in-place patch
    }

    // fast_float::from_chars requires a NUL-terminated range; temporarily NUL-terminate
    char saved = *line_end;
    *const_cast<char*>(line_end) = '\0';

    const char* last = nullptr;
    auto [ptr, ec] = fast_float::from_chars(p, line_end + 1, value);
    *const_cast<char*>(line_end) = saved; // restore
    if (d_pos) *const_cast<char*>(d_pos) = patch; // restore

    if (ec != std::errc{} || ptr == p) return start; // parse failed
    return line_end;
}

} // anonymous

double parse_sum(const std::string& input) {
    const char* p   = input.data();
    const char* end = p + input.size();
    double sum = 0.0;

    while (p < end) {
        double v = 0.0;
        const char* next = parse_double(p, end, v);
        if (next == p) {          // no number on this line
            while (next < end && *next != '\n') ++next;
        } else {
            sum += v;
        }
        p = (next < end && *next == '\n') ? next + 1 : next;
    }
    return sum;
}