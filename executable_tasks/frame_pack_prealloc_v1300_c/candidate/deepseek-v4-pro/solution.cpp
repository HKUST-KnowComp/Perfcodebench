#include "interface.h"

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    uint64_t final_hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = 1469598103934665603ULL;
        for (const auto& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            uint8_t b0 = static_cast<uint8_t>(len & 0xFF);
            uint8_t b1 = static_cast<uint8_t>(len >> 8);
            hash ^= b0;
            hash *= 1099511628211ULL;
            hash ^= b1;
            hash *= 1099511628211ULL;
            for (char c : frame) {
                uint8_t b = static_cast<uint8_t>(c);
                hash ^= b;
                hash *= 1099511628211ULL;
            }
        }
        final_hash = hash;
    }
    return final_hash;
}