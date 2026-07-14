#include "interface.h"

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;
    
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = FNV_OFFSET;
        for (const auto& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            uint8_t b0 = static_cast<uint8_t>(len);
            uint8_t b1 = static_cast<uint8_t>(len >> 8);
            
            h ^= b0; h *= FNV_PRIME;
            h ^= b1; h *= FNV_PRIME;
            
            for (char c : frame) {
                h ^= static_cast<uint8_t>(c);
                h *= FNV_PRIME;
            }
        }
        hash = h;
    }
    return hash;
}