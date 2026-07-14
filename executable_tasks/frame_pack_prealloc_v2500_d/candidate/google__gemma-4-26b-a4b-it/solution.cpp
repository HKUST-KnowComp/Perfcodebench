#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

namespace {

// Optimized FNV-1a hash implementation
inline uint64_t checksum_bytes_optimized(const uint8_t* data, size_t size) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    for (size_t i = 0; i < size; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= prime;
    }
    return hash;
}

} // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    if (frames.empty()) {
        // Handle empty case to match baseline behavior
        uint64_t hash = 0;
        for (int iter = 0; iter < iters; ++iter) {
            hash = checksum_bytes_optimized(nullptr, 0);
        }
        return hash;
    }

    // Pre-calculate total size needed to avoid reallocations
    size_t total_size = 0;
    for (const auto& frame : frames) {
        total_size += 2 + frame.size(); // 2 bytes for uint16_t length + frame content
    }

    std::vector<uint8_t> buffer(total_size);
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        size_t offset = 0;
        for (const auto& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            
            // Write length (little-endian as per baseline: v, then v >> 8)
            buffer[offset++] = static_cast<uint8_t>(len);
            buffer[offset++] = static_cast<uint8_t>(len >> 8);
            
            // Bulk copy frame content
            if (len > 0) {
                std::memcpy(&buffer[offset], frame.data(), len);
                offset += len;
            }
        }
        hash = checksum_bytes_optimized(buffer.data(), total_size);
    }

    return hash;
}