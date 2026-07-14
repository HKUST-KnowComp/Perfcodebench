#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    const uint64_t fnv_offset = 1469598103934665603ULL;
    const uint64_t fnv_prime = 1099511628211ULL;
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = fnv_offset;
        for (const std::string& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            // Write length little-endian (low byte first)
            hash ^= static_cast<uint64_t>(len & 0xFF);
            hash *= fnv_prime;
            hash ^= static_cast<uint64_t>((len >> 8) & 0xFF);
            hash *= fnv_prime;
            // Write frame bytes
            const char* data = frame.data();
            size_t sz = frame.size();
            for (size_t i = 0; i < sz; ++i) {
                hash ^= static_cast<uint64_t>(static_cast<uint8_t>(data[i]));
                hash *= fnv_prime;
            }
        }
    }
    return hash;
}
