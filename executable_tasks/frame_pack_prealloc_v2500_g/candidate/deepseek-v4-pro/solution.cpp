#include "interface.h"
#include <vector>
#include <string>
#include <cstdint>

namespace {
// No helper functions needed.
} // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    const uint64_t init_hash = 1469598103934665603ULL;
    const uint64_t multiplier = 1099511628211ULL;
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = init_hash;
        for (const std::string& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            // Low byte
            uint8_t b0 = static_cast<uint8_t>(len);
            h ^= b0;
            h *= multiplier;
            // High byte
            uint8_t b1 = static_cast<uint8_t>(len >> 8);
            h ^= b1;
            h *= multiplier;
            // Frame data
            for (char c : frame) {
                uint8_t b = static_cast<uint8_t>(c);
                h ^= b;
                h *= multiplier;
            }
        }
        hash = h;
    }
    return hash;
}