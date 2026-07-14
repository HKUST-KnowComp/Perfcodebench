#include "interface.h"
#include <vector>
#include <cstring>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    if (iters <= 0) return 0;

    size_t total_size = 0;
    for (const auto& frame : frames) {
        total_size += 2 + frame.size();
    }

    std::vector<uint8_t> buffer(total_size);
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* ptr = buffer.data();
        for (const std::string& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            *ptr++ = static_cast<uint8_t>(len);
            *ptr++ = static_cast<uint8_t>(len >> 8);
            if (len > 0) {
                std::memcpy(ptr, frame.data(), len);
                ptr += len;
            }
        }

        // FNV-1a checksum
        uint64_t current_hash = 1469598103934665603ULL;
        const uint8_t* data = buffer.data();
        size_t size = buffer.size();
        for (size_t i = 0; i < size; ++i) {
            current_hash ^= static_cast<uint64_t>(data[i]);
            current_hash *= 1099511628211ULL;
        }
        hash = current_hash;
    }

    return hash;
}