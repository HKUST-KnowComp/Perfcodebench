#include "interface.h"

#include <vector>
#include <string>
#include <cstdint>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    if (iters <= 0) return 0;

    // Pre-calculate total size to avoid reallocations
    size_t total_size = 0;
    for (const auto& frame : frames) {
        total_size += 2 + frame.size();
    }

    // Pre-allocate buffer
    std::vector<uint8_t> buffer(total_size);
    
    // We only need the result of the last iteration's checksum based on the baseline logic
    // However, the baseline calculates hash = checksum_bytes(out) inside the loop,
    // effectively overwriting the previous hash. 
    // The baseline's 'hash' variable is updated in each iteration.
    
    uint64_t final_hash = 0;
    const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* ptr = buffer.data();
        
        // Pack frames into the buffer
        for (const std::string& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            *ptr++ = static_cast<uint8_t>(len & 0xFF);
            *ptr++ = static_cast<uint8_t>((len >> 8) & 0xFF);
            
            if (!frame.empty()) {
                // Use memcpy for fast block transfer
                __builtin_memcpy(ptr, frame.data(), frame.size());
                ptr += frame.size();
            }
        }

        // Calculate checksum of the packed buffer
        uint64_t hash = FNV_OFFSET_BASIS;
        const uint8_t* data_ptr = buffer.data();
        const uint8_t* end_ptr = ptr;
        
        while (data_ptr < end_ptr) {
            hash ^= static_cast<uint64_t>(*data_ptr++);
            hash *= FNV_PRIME;
        }
        final_hash = hash;
    }

    return final_hash;
}