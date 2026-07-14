#include "interface.h"
#include <vector>
#include <string>
#include <cstring>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    size_t total_size = 0;
    for (const auto& frame : frames) {
        total_size += frame.size() + 2;
    }

    std::vector<uint8_t> buffer(total_size);
    uint8_t* ptr = buffer.data();
    for (const auto& frame : frames) {
        uint16_t len = static_cast<uint16_t>(frame.size());
        std::memcpy(ptr, &len, 2);
        std::memcpy(ptr + 2, frame.data(), len);
        ptr += len + 2;
    }

    uint64_t hash = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (size_t i = 0; i < total_size; ++i) {
            hash ^= static_cast<uint64_t>(buffer[i]);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}