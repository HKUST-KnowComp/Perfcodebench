#include "interface.h"
#include "is_utf8.h"
#include <cstdint>
#include <cstring>

namespace {

bool validate_utf8_fast(const std::string& s) {
    const uint8_t* p = reinterpret_cast<const uint8_t*>(s.data());
    const uint8_t* end = p + s.size();

    // Fast path: process 8 bytes at a time for pure ASCII
    while (p + 8 <= end) {
        uint64_t chunk;
        std::memcpy(&chunk, p, 8);
        if ((chunk & 0x8080808080808080ULL) == 0) {
            p += 8;
        } else {
            break;
        }
    }

    while (p < end) {
        uint8_t c = *p++;
        if (c <= 0x7F) continue;

        if (c >= 0xC2 && c <= 0xDF) {
            if (p >= end || (p[0] & 0xC0) != 0x80) return false;
            ++p;
            continue;
        }
        if (c == 0xE0) {
            if (end - p < 2 || p[0] < 0xA0 || p[0] > 0xBF || (p[1] & 0xC0) != 0x80) return false;
            p += 2;
            continue;
        }
        if (c >= 0xE1 && c <= 0xEC) {
            if (end - p < 2 || (p[0] & 0xC0) != 0x80 || (p[1] & 0xC0) != 0x80) return false;
            p += 2;
            continue;
        }
        if (c == 0xED) {
            if (end - p < 2 || p[0] < 0x80 || p[0] > 0x9F || (p[1] & 0xC0) != 0x80) return false;
            p += 2;
            continue;
        }
        if (c >= 0xEE && c <= 0xEF) {
            if (end - p < 2 || (p[0] & 0xC0) != 0x80 || (p[1] & 0xC0) != 0x80) return false;
            p += 2;
            continue;
        }
        if (c == 0xF0) {
            if (end - p < 3 || p[0] < 0x90 || p[0] > 0xBF || (p[1] & 0xC0) != 0x80 || (p[2] & 0xC0) != 0x80) return false;
            p += 3;
            continue;
        }
        if (c >= 0xF1 && c <= 0xF3) {
            if (end - p < 3 || (p[0] & 0xC0) != 0x80 || (p[1] & 0xC0) != 0x80 || (p[2] & 0xC0) != 0x80) return false;
            p += 3;
            continue;
        }
        if (c == 0xF4) {
            if (end - p < 3 || p[0] < 0x80 || p[0] > 0x8F || (p[1] & 0xC0) != 0x80 || (p[2] & 0xC0) != 0x80) return false;
            p += 3;
            continue;
        }
        return false;
    }
    return true;
}

} // namespace

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
    std::size_t count = 0;
    for (const auto& s : inputs) {
        count += validate_utf8_fast(s) ? 1U : 0U;
    }
    return count;
}
