#include "interface.h"
#include <vector>
#include <string>
#include <cstring>
#include <cstdint>

namespace {

// Optimized FNV-1a checksum implementation working on raw memory
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
        // If no frames, the packed buffer is empty. FNV-1a of empty is the offset basis.
        return 1469598103934665603ULL;
    }

    // Pre-calculate total size needed for the buffer to avoid reallocations
    // Each frame adds 2 bytes for the uint16_t length prefix
    size_t total_size = 0;
    for (const auto& frame : frames) {
        total_size += 2 + frame.size();
    }

    // Pre-allocate buffer once
    std::vector<uint8_t> buffer(total_size);
    uint8_t* ptr = buffer.data();
    uint64_t last_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* current = ptr;
        for (const auto& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            
            // Write length (little-endian as per original write_len)
            *reinterpret_cast<uint16_t*>(current) = len;
            current += 2;
            
            // Write frame content
            if (len > 0) {
                std::memcpy(current, frame.data(), len);
                current += len;
            }
        }
        // Calculate checksum on the packed buffer
        last_hash = checksum_bytes_fast(ptr, total_size);
    }

    return last_hash;
}
