#include "interface.h"

#include <vector>
#include <string>
#include <cstdint>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    if (iters <= 0) return 0;

    // Pre-calculate total size needed for one iteration
    size_t total_size = 0;
    for (const auto& frame : frames) {
        total_size += 2 + frame.size();
    }

    // Pre-allocate buffer to avoid reallocations
    std::vector<uint8_t> buffer(total_size);
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* ptr = buffer.data();
        
        // Pack frames into the buffer
        for (const std::string& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            *ptr++ = static_cast<uint8_t>(len);
            *ptr++ = static_cast<uint8_t>(len >> 8);
            
            const char* data = frame.data();
            for (size_t i = 0; i < frame.size(); ++i) {
                *ptr++ = static_cast<uint8_t>(data[i]);
            }
        }

        // Calculate checksum on the packed buffer
        uint64_t current_hash = 1469598103934665603ULL;
        const uint8_t* hash_ptr = buffer.data();
        for (size_t i = 0; i < total_size; ++i) {
            current_hash ^= static_cast<uint64_t>(hash_ptr[i]);
            current_hash *= 1099511628211ULL;
        }
        hash = current_hash;
    }

    return hash;
}