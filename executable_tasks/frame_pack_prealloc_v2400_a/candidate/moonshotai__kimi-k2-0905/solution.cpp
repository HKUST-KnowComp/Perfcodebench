#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>

namespace {

inline uint64_t update_fnv1a(uint64_t h, uint8_t b) {
    h ^= static_cast<uint64_t>(b);
    h *= 1099511628211ULL;
    return h;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    // one-time size calculation
    size_t total = 0;
    for (const auto& f : frames) total += 2 + f.size();

    std::vector<uint8_t> buf;
    buf.reserve(total);

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        buf.clear();
        uint64_t h = 1469598103934665603ULL;

        for (const auto& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            uint8_t lo = static_cast<uint8_t>(len);
            uint8_t hi = static_cast<uint8_t>(len >> 8);
            h = update_fnv1a(h, lo);
            h = update_fnv1a(h, hi);
            buf.push_back(lo);
            buf.push_back(hi);

            const char* p = frame.data();
            size_t n = frame.size();
            for (size_t i = 0; i < n; ++i) {
                uint8_t b = static_cast<uint8_t>(p[i]);
                h = update_fnv1a(h, b);
                buf.push_back(b);
            }
        }
        hash = h;
    }
    return hash;
}