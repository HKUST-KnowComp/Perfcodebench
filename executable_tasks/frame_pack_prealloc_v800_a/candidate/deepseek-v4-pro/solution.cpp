#include "interface.h"

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    if (iters <= 0) return 0;
    uint64_t hash = 1469598103934665603ULL;
    constexpr uint64_t multiplier = 1099511628211ULL;
    for (const auto& frame : frames) {
        uint16_t len = static_cast<uint16_t>(frame.size());
        uint8_t b0 = static_cast<uint8_t>(len);
        uint8_t b1 = static_cast<uint8_t>(len >> 8);
        hash ^= static_cast<uint64_t>(b0);
        hash *= multiplier;
        hash ^= static_cast<uint64_t>(b1);
        hash *= multiplier;
        for (uint8_t c : frame) {
            hash ^= static_cast<uint64_t>(c);
            hash *= multiplier;
        }
    }
    return hash;
}
