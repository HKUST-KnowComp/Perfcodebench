#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    uint64_t hash = 0;
    constexpr uint64_t kInitialHash = 1469598103934665603ULL;
    constexpr uint64_t kMultiplier = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t current_hash = kInitialHash;
        for (const std::string& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            // little-endian length: low byte then high byte
            current_hash ^= static_cast<uint64_t>(len & 0xFF);
            current_hash *= kMultiplier;
            current_hash ^= static_cast<uint64_t>(len >> 8);
            current_hash *= kMultiplier;
            // frame payload
            for (unsigned char c : frame) {
                current_hash ^= static_cast<uint64_t>(c);
                current_hash *= kMultiplier;
            }
        }
        hash = current_hash;
    }
    return hash;
}