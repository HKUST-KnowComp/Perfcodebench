#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline uint64_t fnv1a_next(uint64_t hash, uint8_t byte) {
    hash ^= byte;
    hash *= 1099511628211ULL;
    return hash;
}

} // anonymous namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    const uint64_t fnv_offset = 1469598103934665603ULL;
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = fnv_offset;
        for (const std::string& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            // Write length prefix (little-endian)
            hash = fnv1a_next(hash, static_cast<uint8_t>(len & 0xFF));
            hash = fnv1a_next(hash, static_cast<uint8_t>((len >> 8) & 0xFF));
            // Write frame data bytes
            const char* data = frame.data();
            size_t sz = frame.size();
            for (size_t i = 0; i < sz; ++i) {
                hash = fnv1a_next(hash, static_cast<uint8_t>(data[i]));
            }
        }
        final_hash = hash;
    }
    return final_hash;
}
