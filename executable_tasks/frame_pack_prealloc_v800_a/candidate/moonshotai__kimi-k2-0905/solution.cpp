#include "interface.h"
#include <vector>
#include <cstring>

namespace {

inline uint64_t checksum_bytes(const uint8_t* __restrict p, size_t n) {
    const uint64_t prime = 1099511628211ULL;
    uint64_t hash = 1469598103934665603ULL;

    // process 8 bytes at a time
    while (n >= 8) {
        uint64_t v;
        std::memcpy(&v, p, 8);
        const uint8_t* vp = reinterpret_cast<const uint8_t*>(&v);
        for (unsigned i = 0; i < 8; ++i) {
            hash ^= vp[i];
            hash *= prime;
        }
        p += 8;
        n -= 8;
    }
    // tail
    while (n--) {
        hash ^= *p++;
        hash *= prime;
    }
    return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    // one-time size calculation
    size_t total = 0;
    for (const auto& f : frames) total += 2 + f.size();

    std::vector<uint8_t> out;
    out.reserve(total);

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        out.clear();
        // fast append
        for (const auto& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            out.push_back(static_cast<uint8_t>(len));
            out.push_back(static_cast<uint8_t>(len >> 8));
            out.insert(out.end(), frame.data(), frame.data() + frame.size());
        }
        hash = checksum_bytes(out.data(), out.size());
    }
    return hash;
}