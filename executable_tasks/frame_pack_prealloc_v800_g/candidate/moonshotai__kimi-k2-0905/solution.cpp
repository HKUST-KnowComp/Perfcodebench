#include "interface.h"
#include <vector>
#include <cstring>

namespace {

inline uint64_t checksum_bytes(const uint8_t* __restrict data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* end = data + len;
    while (data != end) {
        hash ^= static_cast<uint64_t>(*data++);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    // Pre-compute total size
    size_t total = 0;
    for (const std::string& f : frames) {
        total += 2 + f.size(); // 2 bytes length + payload
    }

    std::vector<uint8_t> out;
    out.reserve(total);

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        out.resize(total);                  // reuse buffer
        uint8_t* dst = out.data();

        for (const std::string& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            // little-endian 16-bit write
            std::memcpy(dst, &len, 2);
            dst += 2;
            std::memcpy(dst, frame.data(), frame.size());
            dst += frame.size();
        }

        hash = checksum_bytes(out.data(), total);
    }
    return hash;
}