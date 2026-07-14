#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL;
        for (const auto& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            h ^= static_cast<uint8_t>(len);
            h *= 1099511628211ULL;
            h ^= static_cast<uint8_t>(len >> 8);
            h *= 1099511628211ULL;
            for (char c : frame) {
                h ^= static_cast<uint8_t>(c);
                h *= 1099511628211ULL;
            }
        }
        hash = h;
    }
    return hash;
}