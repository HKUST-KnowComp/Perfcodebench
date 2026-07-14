#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    uint64_t hash = 0;
    constexpr uint64_t fnv_prime = 1099511628211ULL;
    
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL;
        for (const std::string& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            
            h ^= static_cast<uint64_t>(len & 0xFF);
            h *= fnv_prime;
            h ^= static_cast<uint64_t>(len >> 8);
            h *= fnv_prime;
            
            const char* data = frame.data();
            for (size_t i = 0, n = frame.size(); i < n; ++i) {
                h ^= static_cast<uint64_t>(static_cast<uint8_t>(data[i]));
                h *= fnv_prime;
            }
        }
        hash = h;
    }
    return hash;
}
