#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t h = 1469598103934665603ULL;
        for (const std::string& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            uint8_t b0 = static_cast<uint8_t>(len);
            uint8_t b1 = static_cast<uint8_t>(len >> 8);
            
            h ^= static_cast<uint64_t>(b0); h *= 1099511628211ULL;
            h ^= static_cast<uint64_t>(b1); h *= 1099511628211ULL;
            
            const char* ptr = frame.data();
            const char* end = ptr + frame.size();
            while (ptr != end) {
                h ^= static_cast<uint64_t>(static_cast<uint8_t>(*ptr++));
                h *= 1099511628211ULL;
            }
        }
        hash = h;
    }
    return hash;
}