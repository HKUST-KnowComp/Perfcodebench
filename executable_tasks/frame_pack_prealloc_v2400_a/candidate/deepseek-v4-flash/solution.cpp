#include "interface.h"

namespace {

inline uint64_t fnv1a_update(uint64_t hash, uint8_t byte) {
    hash ^= static_cast<uint64_t>(byte);
    hash *= 1099511628211ULL;
    return hash;
}

} // unnamed namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    const uint64_t fnv_offset = 1469598103934665603ULL;
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = fnv_offset;
        for (const std::string& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            hash = fnv1a_update(hash, static_cast<uint8_t>(len & 0xFF));
            hash = fnv1a_update(hash, static_cast<uint8_t>((len >> 8) & 0xFF));
            for (char c : frame) {
                hash = fnv1a_update(hash, static_cast<uint8_t>(c));
            }
        }
    }
    return hash;
}
