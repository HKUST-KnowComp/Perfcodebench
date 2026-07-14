#include "interface.h"
#include <vector>
#include <cstring>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    size_t total_size = 0;
    for (const auto& f : frames) {
        total_size += f.size() + 2;
    }

    std::vector<uint8_t> buffer(total_size);
    uint8_t* ptr = buffer.data();
    for (const auto& f : frames) {
        uint16_t len = static_cast<uint16_t>(f.size());
        *ptr++ = static_cast<uint8_t>(len);
        *ptr++ = static_cast<uint8_t>(len >> 8);
        std::memcpy(ptr, f.data(), f.size());
        ptr += f.size();
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (size_t i = 0; i < total_size; ++i) {
            hash ^= static_cast<uint64_t>(buffer[i]);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}