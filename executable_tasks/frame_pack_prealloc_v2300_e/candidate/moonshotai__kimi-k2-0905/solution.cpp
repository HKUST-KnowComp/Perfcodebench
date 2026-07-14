#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

namespace {

inline uint64_t checksum_bytes(const uint8_t* __restrict p, size_t n) {
    uint64_t h = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    // process 8 bytes at a time
    const uint8_t* end8 = p + (n & ~size_t(7));
    for (; p != end8; p += 8) {
        uint64_t v;
        std::memcpy(&v, p, 8);        // unaligned safe
        h ^= v;
        h *= prime;
    }
    // tail
    for (size_t i = 0, rem = n & 7; i < rem; ++i) {
        h ^= uint64_t(p[i]);
        h *= prime;
    }
    return h;
}

} // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    // compute exact packed size once
    size_t total = 0;
    for (const auto& f : frames) total += 2 + f.size();

    std::vector<uint8_t> out;
    out.reserve(total);

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        out.clear();
        uint8_t* dst = out.data();
        for (const auto& f : frames) {
            uint16_t len = static_cast<uint16_t>(f.size());
            std::memcpy(dst, &len, 2);        // little-endian store
            dst += 2;
            std::memcpy(dst, f.data(), f.size());
            dst += f.size();
        }
        hash = checksum_bytes(out.data(), total);
    }
    return hash;
}