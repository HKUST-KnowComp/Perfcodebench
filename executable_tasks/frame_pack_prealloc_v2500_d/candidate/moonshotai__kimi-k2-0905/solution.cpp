#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace {

inline uint64_t checksum_bytes(const uint8_t* __restrict data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* end = data + len;
    for (; data != end; ++data) {
        hash ^= *data;
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    // single pass to compute exact required buffer size
    size_t total = 0;
    for (const std::string& f : frames) {
        total += 2 + f.size(); // 2 bytes length + payload
    }

    std::vector<uint8_t> out;
    out.reserve(total);

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        out.resize(total); // reuse existing memory
        uint8_t* dst = out.data();

        for (const std::string& f : frames) {
            uint16_t len = static_cast<uint16_t>(f.size());
            // unaligned 16-bit store: safe on x86-64
            *reinterpret_cast<uint16_t*>(dst) = len;
            dst += 2;
            std::memcpy(dst, f.data(), len);
            dst += len;
        }

        hash = checksum_bytes(out.data(), total);
    }
    return hash;
}