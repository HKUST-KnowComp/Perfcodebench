#include "interface.h"

#include <cstdint>
#include <vector>
#include <string>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    if (iters <= 0) return 0;
    
    uint64_t hash = 1469598103934665603ULL;
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;
    
    for (const auto& frame : frames) {
        uint16_t len = static_cast<uint16_t>(frame.size());
        
        // Process length bytes (little-endian, matching baseline write_len)
        hash ^= static_cast<uint64_t>(len & 0xFF);
        hash *= FNV_PRIME;
        hash ^= static_cast<uint64_t>(len >> 8);
        hash *= FNV_PRIME;
        
        // Process frame payload bytes directly
        for (unsigned char c : frame) {
            hash ^= static_cast<uint64_t>(c);
            hash *= FNV_PRIME;
        }
    }
    
    return hash;
}