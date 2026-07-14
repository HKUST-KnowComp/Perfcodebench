#include "interface.h"

#include <vector>
#include <cstdint>
#include <cstring>

namespace {

inline uint64_t checksum_bytes(const uint8_t* data, size_t len) {
    const uint64_t prime = 1099511628211ULL;
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* end = data + len;
    while (data + 8 <= end) {
        uint64_t chunk;
        std::memcpy(&chunk, data, 8);
        hash ^= chunk;
        hash *= prime;
        data += 8;
    }
    while (data < end) {
        hash ^= static_cast<uint64_t>(*data++);
        hash *= prime;
    }
    return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    size_t total = 0;
    for (const auto& f : frames) total += f.size() + 2;

    std::vector<uint8_t> out;
    out.reserve(total);

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        out.resize(total);
        uint8_t* dst = out.data();
        for (const auto& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            dst[0] = static_cast<uint8_t>(len);
            dst[1] = static_cast<uint8_t>(len >> 8);
            dst += 2;
            std::memcpy(dst, frame.data(), len);
            dst += len;
        }
        hash = checksum_bytes(out.data(), out.size());
    }
    return hash;
}