#include "interface.h"
#include <vector>
#include <string>
#include <cstring>
#include <cstdint>

namespace {

// Optimized FNV-1a checksum implementation using raw pointer access
inline uint64_t checksum_bytes_fast(const uint8_t* data, size_t size) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < size; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    if (frames.empty()) {
        // Handle empty input case to match baseline behavior
        uint64_t hash = 0;
        for (int iter = 0; iter < iters; ++iter) {
            hash = checksum_bytes_fast(nullptr, 0);
        }
        return hash;
    }

    // Pre-calculate total size needed to avoid reallocations
    size_t total_size = 0;
    for (const auto& frame : frames) {
        total_size += 2 + frame.size(); // 2 bytes for uint16_t length + frame content
    }

    std::vector<uint8_t> buffer(total_size);
    uint64_t last_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* ptr = buffer.data();
        for (const auto& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            
            // Manual write of little-endian length
            *ptr++ = static_cast<uint8_t>(len);
            *ptr++ = static_cast<uint8_t>(len >> 8);
            
            // Fast copy of frame content
            if (len > 0) {
                std::memcpy(ptr, frame.data(), len);
                ptr += len;
            }
        }
        last_hash = checksum_bytes_fast(buffer.data(), total_size);
    }

    return last_hash;
}