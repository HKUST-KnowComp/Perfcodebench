#include "interface.h"
#include <vector>
#include <string>
#include <cstdint>
#include <cstring>

namespace {

// Optimized FNV-1a implementation using a raw pointer to avoid vector overhead
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
    if (iters <= 0) return 0;

    // 1. Pre-calculate total size to perform a single allocation
    size_t total_size = 0;
    for (const auto& frame : frames) {
        total_size += 2 + frame.size(); // 2 bytes for uint16_t length + frame content
    }

    if (total_size == 0) {
        // Handle empty case if necessary, though FNV-1a on empty is the offset basis
        uint64_t hash = 1469598103934665603ULL;
        for (int iter = 0; iter < iters; ++iter) {
            hash = checksum_bytes_fast(nullptr, 0);
        }
        return hash;
    }

    std::vector<uint8_t> buffer(total_size);
    uint8_t* ptr = buffer.data();

    // 2. Pack the buffer once (since frames are constant across iterations)
    for (const auto& frame : frames) {
        uint16_t len = static_cast<uint16_t>(frame.size());
        // Little-endian length encoding as per baseline: [low, high]
        ptr[0] = static_cast<uint8_t>(len);
        ptr[1] = static_cast<uint8_t>(len >> 8);
        ptr += 2;
        
        if (len > 0) {
            std::memcpy(ptr, frame.data(), len);
            ptr += len;
        }
    }

    // 3. Run the checksum iterations
    // Since the packed buffer is identical every iteration, we can optimize the loop.
    // However, to strictly follow the logic of 're-calculating' as per the baseline:
    uint64_t last_hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        last_hash = checksum_bytes_fast(buffer.data(), total_size);
    }

    return last_hash;
}