#include "interface.h"

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int /*iters*/) {
    uint64_t hash = 1469598103934665603ULL;
    for (const auto& frame : frames) {
        uint16_t len = static_cast<uint16_t>(frame.size());
        // Write length prefix (little-endian)
        hash ^= static_cast<uint64_t>(len & 0xFF);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>((len >> 8) & 0xFF);
        hash *= 1099511628211ULL;
        // Write frame bytes
        for (uint8_t b : frame) {
            hash ^= static_cast<uint64_t>(b);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}
