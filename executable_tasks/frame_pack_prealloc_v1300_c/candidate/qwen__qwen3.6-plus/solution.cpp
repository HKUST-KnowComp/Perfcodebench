#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;
    constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
    
    uint64_t result = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = FNV_OFFSET;
        for (const std::string& frame : frames) {
            size_t sz = frame.size();
            uint16_t len = static_cast<uint16_t>(sz);
            
            h ^= static_cast<uint64_t>(len & 0xFF);
            h *= FNV_PRIME;
            h ^= static_cast<uint64_t>((len >> 8) & 0xFF);
            h *= FNV_PRIME;
            
            const uint8_t* data = reinterpret_cast<const uint8_t*>(frame.data());
            const uint8_t* end = data + sz;
            while (data != end) {
                h ^= static_cast<uint64_t>(*data++);
                h *= FNV_PRIME;
            }
        }
        result = h;
    }
    return result;
}