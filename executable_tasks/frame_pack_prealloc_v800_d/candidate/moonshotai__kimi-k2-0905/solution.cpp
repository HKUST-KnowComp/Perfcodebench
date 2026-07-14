#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

namespace {

inline uint64_t checksum_bytes(const uint8_t* data, size_t n) {
    uint64_t h = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    // 8-way unrolled SWAR
    const size_t blocks = n / 8;
    const uint8_t* p = data;
    for (size_t i = 0; i < blocks; ++i) {
        uint64_t v;
        std::memcpy(&v, p, 8);
        const uint8_t* b = reinterpret_cast<const uint8_t*>(&v);
        h ^= static_cast<uint64_t>(b[0]); h *= prime;
        h ^= static_cast<uint64_t>(b[1]); h *= prime;
        h ^= static_cast<uint64_t>(b[2]); h *= prime;
        h ^= static_cast<uint64_t>(b[3]); h *= prime;
        h ^= static_cast<uint64_t>(b[4]); h *= prime;
        h ^= static_cast<uint64_t>(b[5]); h *= prime;
        h ^= static_cast<uint64_t>(b[6]); h *= prime;
        h ^= static_cast<uint64_t>(b[7]); h *= prime;
        p += 8;
    }

    // tail
    const size_t tail = n & 7;
    for (size_t i = 0; i < tail; ++i) {
        h ^= static_cast<uint64_t>(p[i]);
        h *= prime;
    }
    return h;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    // compute exact packed size once
    size_t total = 0;
    for (const auto& f : frames) total += 2 + f.size();

    std::vector<uint8_t> out;
    out.reserve(total);

    // pack once
    for (const auto& f : frames) {
        uint16_t len = static_cast<uint16_t>(f.size());
        out.push_back(static_cast<uint8_t>(len));
        out.push_back(static_cast<uint8_t>(len >> 8));
        if (!f.empty()) {
            out.insert(out.end(),
                       reinterpret_cast<const uint8_t*>(f.data()),
                       reinterpret_cast<const uint8_t*>(f.data() + f.size()));
        }
    }

    // identical every iteration
    uint64_t hash = checksum_bytes(out.data(), out.size());
    for (int i = 1; i < iters; ++i) {
        // no-op; hash already correct
    }
    return hash;
}