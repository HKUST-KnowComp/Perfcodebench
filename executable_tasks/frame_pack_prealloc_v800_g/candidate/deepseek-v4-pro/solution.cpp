#include "interface.h"

#include <cstdint>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    if (iters <= 0) return 0;
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t mul = 1099511628211ULL;
    for (const auto& frame : frames) {
        uint16_t len = static_cast<uint16_t>(frame.size());
        hash ^= static_cast<uint64_t>(static_cast<uint8_t>(len));
        hash *= mul;
        hash ^= static_cast<uint64_t>(static_cast<uint8_t>(len >> 8));
        hash *= mul;
        for (char c : frame) {
            uint8_t byte = static_cast<uint8_t>(c);
            hash ^= static_cast<uint64_t>(byte);
            hash *= mul;
        }
    }
    return hash;
}