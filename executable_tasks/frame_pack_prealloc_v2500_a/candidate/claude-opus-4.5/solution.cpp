#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    // Precompute total size: 2 bytes length prefix per frame + frame data
    size_t total_size = 0;
    for (const std::string& frame : frames) {
        total_size += 2 + frame.size();
    }

    // Allocate buffer once
    std::vector<uint8_t> out(total_size);
    uint8_t* const base = out.data();

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* ptr = base;
        for (const std::string& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            // Write little-endian length prefix
            ptr[0] = static_cast<uint8_t>(len);
            ptr[1] = static_cast<uint8_t>(len >> 8);
            ptr += 2;
            // Copy frame data
            std::memcpy(ptr, frame.data(), frame.size());
            ptr += frame.size();
        }

        // FNV-1a checksum inline
        constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
        constexpr uint64_t FNV_PRIME = 1099511628211ULL;
        hash = FNV_OFFSET;
        const uint8_t* p = base;
        const uint8_t* end = base + total_size;
        while (p != end) {
            hash ^= static_cast<uint64_t>(*p++);
            hash *= FNV_PRIME;
        }
    }
    return hash;
}