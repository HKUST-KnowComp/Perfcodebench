#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    const uint64_t seed = 1469598103934665603ULL;
    const uint64_t mul = 1099511628211ULL;
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = seed;
        for (const auto& frame : frames) {
            const uint16_t len = static_cast<uint16_t>(frame.size());
            // little-endian length prefix
            h ^= static_cast<uint64_t>(len & 0xFF);
            h *= mul;
            h ^= static_cast<uint64_t>(len >> 8);
            h *= mul;
            // frame data
            for (unsigned char c : frame) {
                h ^= static_cast<uint64_t>(c);
                h *= mul;
            }
        }
        hash = h;
    }
    return hash;
}
