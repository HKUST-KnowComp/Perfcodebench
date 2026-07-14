#include "interface.h"

#include <vector>
#include <cstdint>
#include <algorithm>

namespace {

inline uint64_t checksum_bytes(const uint8_t* first, const uint8_t* last) noexcept {
    uint64_t hash = 1469598103934665603ULL;
    while (first != last) {
        hash ^= *first++;
        hash *= 1099511628211ULL;
    }
    return hash;
}

inline void write_len(uint8_t*& dst, uint16_t v) noexcept {
    dst[0] = static_cast<uint8_t>(v);
    dst[1] = static_cast<uint8_t>(v >> 8);
    dst += 2;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    // Exact size needed for the entire stream: 2 bytes per frame + frame bytes
    size_t total = 0;
    for (const auto& f : frames) total += 2 + f.size();

    std::vector<uint8_t> out;
    out.resize(total);              // allocate once

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* pos = out.data();

        for (const auto& frame : frames) {
            const uint16_t len = static_cast<uint16_t>(frame.size());
            write_len(pos, len);
            std::copy(frame.data(), frame.data() + len, pos);
            pos += len;
        }

        hash = checksum_bytes(out.data(), pos);
    }
    return hash;
}
