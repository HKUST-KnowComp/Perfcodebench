#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    uint64_t hash = 0;
    constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
    constexpr uint64_t FNV_PRIME  = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (const auto& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            
            hash ^= static_cast<uint64_t>(static_cast<uint8_t>(len));
            hash *= FNV_PRIME;
            hash ^= static_cast<uint64_t>(static_cast<uint8_t>(len >> 8));
            hash *= FNV_PRIME;
            
            const char* d = frame.data();
            for (size_t i = 0, n = frame.size(); i < n; ++i) {
                hash ^= static_cast<uint64_t>(static_cast<uint8_t>(d[i]));
                hash *= FNV_PRIME;
            }
        }
    }
    return hash;
}
