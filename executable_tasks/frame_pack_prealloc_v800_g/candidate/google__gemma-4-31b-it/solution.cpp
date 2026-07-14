#include "interface.h"

#include <vector>
#include <string>
#include <cstdint>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    if (iters <= 0) return 0;

    // Pre-calculate total size to allocate buffer once
    size_t total_size = 0;
    for (const auto& frame : frames) {
        total_size += 2 + frame.size();
    }

    std::vector<uint8_t> buffer(total_size);
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* ptr = buffer.data();
        
        for (const std::string& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            
            // Pack length (little-endian)
            *ptr++ = static_cast<uint8_t>(len);
            *ptr++ = static_cast<uint8_t>(len >> 8);
            
            // Pack frame data
            const char* data = frame.data();
            size_t s = frame.size();
            for (size_t i = 0; i < s; ++i) {
                *ptr++ = static_cast<uint8_t>(data[i]);
            }
        }

        // Compute FNV-1a checksum on the packed buffer
        uint64_t hash = 1469598103934665603ULL;
        const uint8_t* hash_ptr = buffer.data();
        for (size_t i = 0; i < total_size; ++i) {
            hash ^= static_cast<uint64_t>(hash_ptr[i]);
            hash *= 1099511628211ULL;
        }
        final_hash = hash;
    }

    return final_hash;
}