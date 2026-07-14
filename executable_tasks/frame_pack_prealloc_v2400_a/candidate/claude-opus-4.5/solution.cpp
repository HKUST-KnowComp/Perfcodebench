#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    // Precompute total size needed: 2 bytes length prefix per frame + frame data
    size_t total_size = 0;
    for (const std::string& frame : frames) {
        total_size += 2 + frame.size();
    }

    // Preallocate buffer once
    std::vector<uint8_t> out(total_size);
    uint8_t* const base = out.data();

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* ptr = base;

        // Pack all frames: length prefix (little-endian 16-bit) + data
        for (const std::string& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            ptr[0] = static_cast<uint8_t>(len);
            ptr[1] = static_cast<uint8_t>(len >> 8);
            ptr += 2;
            std::memcpy(ptr, frame.data(), frame.size());
            ptr += frame.size();
        }

        // FNV-1a checksum with manual unrolling
        constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
        constexpr uint64_t FNV_PRIME = 1099511628211ULL;

        hash = FNV_OFFSET;
        const uint8_t* p = base;
        const uint8_t* end = base + total_size;

        // Process 8 bytes at a time
        while (p + 8 <= end) {
            hash ^= static_cast<uint64_t>(p[0]);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint64_t>(p[1]);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint64_t>(p[2]);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint64_t>(p[3]);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint64_t>(p[4]);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint64_t>(p[5]);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint64_t>(p[6]);
            hash *= FNV_PRIME;
            hash ^= static_cast<uint64_t>(p[7]);
            hash *= FNV_PRIME;
            p += 8;
        }

        // Handle remaining bytes
        while (p < end) {
            hash ^= static_cast<uint64_t>(*p++);
            hash *= FNV_PRIME;
        }
    }
    return hash;
}
