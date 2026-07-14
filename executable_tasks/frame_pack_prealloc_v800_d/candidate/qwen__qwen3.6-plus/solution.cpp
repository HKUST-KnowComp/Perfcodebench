#include "interface.h"
#include <vector>
#include <string>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;
    constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
    
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (const auto& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            
            hash ^= static_cast<uint64_t>(len & 0xFF);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint64_t>(len >> 8);
            hash *= FNV_PRIME;
            
            const uint8_t* data = reinterpret_cast<const uint8_t*>(frame.data());
            const uint8_t* end = data + frame.size();
            while (data < end) {
                hash ^= *data++;
                hash *= FNV_PRIME;
            }
        }
    }
    return hash;
}