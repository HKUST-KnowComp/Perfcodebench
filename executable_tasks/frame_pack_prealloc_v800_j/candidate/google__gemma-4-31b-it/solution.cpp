#include "interface.h"

#include <vector>
#include <string>

/**
 * The goal is to compute the FNV-1a hash of a byte stream formed by
 * packing frames as [length_low, length_high, frame_data...].
 * 
 * Instead of allocating a buffer and copying data into it, we can
 * compute the hash incrementally to avoid memory overhead and
 * unnecessary copies.
 */

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    const uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
    const uint64_t FNV_PRIME = 1099511628211ULL;

    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = FNV_OFFSET_BASIS;

        for (const std::string& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            
            // Pack length: low byte then high byte
            uint8_t low = static_cast<uint8_t>(len & 0xFF);
            uint8_t high = static_cast<uint8_t>((len >> 8) & 0xFF);

            hash ^= static_cast<uint64_t>(low);
            hash *= FNV_PRIME;
            
            hash ^= static_cast<uint64_t>(high);
            hash *= FNV_PRIME;

            // Pack frame data
            for (char c : frame) {
                hash ^= static_cast<uint64_t>(static_cast<uint8_t>(c));
                hash *= FNV_PRIME;
            }
        }
        final_hash = hash;
    }

    return final_hash;
}