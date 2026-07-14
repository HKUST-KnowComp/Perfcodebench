#include "interface.h"

#include <vector>
#include <string>

namespace {
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;
} // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    uint64_t final_hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = FNV_OFFSET;
        for (const std::string& frame : frames) {
            const uint16_t len = static_cast<uint16_t>(frame.size());
            // Write little-endian length bytes matching baseline's write_len order
            uint8_t b = static_cast<uint8_t>(len);
            hash ^= static_cast<uint64_t>(b);
            hash *= FNV_PRIME;
            b = static_cast<uint8_t>(len >> 8);
            hash ^= static_cast<uint64_t>(b);
            hash *= FNV_PRIME;
            // Process all frame bytes in original order
            for (unsigned char c : frame) {
                hash ^= static_cast<uint64_t>(c);
                hash *= FNV_PRIME;
            }
        }
        final_hash = hash;
    }
    return final_hash;
}