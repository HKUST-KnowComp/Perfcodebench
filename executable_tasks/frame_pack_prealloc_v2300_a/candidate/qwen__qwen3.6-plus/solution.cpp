#include "interface.h"
#include <cstring>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    size_t total_size = 0;
    for (const auto& f : frames) {
        total_size += f.size() + 2;
    }

    std::vector<uint8_t> out(total_size);
    uint8_t* buf = out.data();
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* p = buf;
        for (const auto& f : frames) {
            uint16_t len = static_cast<uint16_t>(f.size());
            p[0] = static_cast<uint8_t>(len);
            p[1] = static_cast<uint8_t>(len >> 8);
            p += 2;
            std::memcpy(p, f.data(), f.size());
            p += f.size();
        }

        uint64_t h = 1469598103934665603ULL;
        for (size_t i = 0; i < total_size; ++i) {
            h ^= buf[i];
            h *= 1099511628211ULL;
        }
        hash = h;
    }
    return hash;
}