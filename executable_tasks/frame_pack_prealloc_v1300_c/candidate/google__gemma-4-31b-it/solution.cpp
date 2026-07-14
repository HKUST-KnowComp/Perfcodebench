#include "interface.h"

#include <vector>
#include <string>
#include <cstdint>
#include <cstring>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    if (iters <= 0) return 0;

    size_t total_size = 0;
    for (const auto& frame : frames) {
        total_size += 2 + frame.size();
    }

    // Pre-allocate buffer to avoid reallocations
    std::vector<uint8_t> buffer(total_size);
    uint8_t* ptr = buffer.data();

    uint64_t hash = 0;
    const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* current = ptr;
        for (const std::string& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            *current++ = static_cast<uint8_t>(len);
            *current++ = static_cast<uint8_t>(len >> 8);
            
            if (!frame.empty()) {
                std::memcpy(current, frame.data(), frame.size());
                current += frame.size();
            }
        }

        // Calculate checksum on the packed buffer
        uint64_t current_hash = FNV_OFFSET_BASIS;
        for (size_t i = 0; i < total_size; ++i) {
            current_hash ^= static_cast<uint64_t>(buffer[i]);
            current_hash *= FNV_PRIME;
        }
        hash = current_hash;
    }

    return hash;
}