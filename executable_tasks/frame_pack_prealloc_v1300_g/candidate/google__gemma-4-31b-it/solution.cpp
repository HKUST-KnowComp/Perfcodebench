#include "interface.h"
#include <vector>
#include <string>
#include <cstring>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    if (iters <= 0) return 0;

    size_t total_size = 0;
    for (const auto& frame : frames) {
        total_size += 2 + frame.size();
    }

    std::vector<uint8_t> buffer(total_size);
    uint8_t* buf_ptr = buffer.data();
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* current = buf_ptr;
        for (const std::string& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            *current++ = static_cast<uint8_t>(len);
            *current++ = static_cast<uint8_t>(len >> 8);
            if (len > 0) {
                std::memcpy(current, frame.data(), len);
                current += len;
            }
        }

        // FNV-1a hash calculation
        uint64_t h = 1469598103934665603ULL;
        const uint8_t* data = buffer.data();
        size_t size = buffer.size();
        for (size_t i = 0; i < size; ++i) {
            h ^= static_cast<uint64_t>(data[i]);
            h *= 1099511628211ULL;
        }
        hash = h;
    }

    return hash;
}