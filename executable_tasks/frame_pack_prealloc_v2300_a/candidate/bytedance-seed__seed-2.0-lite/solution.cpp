#include "interface.h"
#include <vector>
#include <cstring>

namespace {
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;
}

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    size_t total_size = 0;
    std::vector<std::pair<uint16_t, const char*>> frame_meta;
    frame_meta.reserve(frames.size());
    for (const auto& frame : frames) {
        uint16_t len = static_cast<uint16_t>(frame.size());
        frame_meta.emplace_back(len, frame.data());
        total_size += 2 + len;
    }

    std::vector<uint8_t> out;
    out.reserve(total_size);
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint8_t* dst = out.data();
        uint64_t hash = FNV_OFFSET;

        for (const auto& meta : frame_meta) {
            uint16_t len = meta.first;
            const char* src = meta.second;

            uint8_t low = static_cast<uint8_t>(len);
            uint8_t high = static_cast<uint8_t>(len >> 8);
            *dst++ = low;
            hash ^= low;
            hash *= FNV_PRIME;
            *dst++ = high;
            hash ^= high;
            hash *= FNV_PRIME;

            for (size_t i = 0; i < len; ++i) {
                uint8_t b = static_cast<uint8_t>(src[i]);
                *dst++ = b;
                hash ^= b;
                hash *= FNV_PRIME;
            }
        }
        final_hash = hash;
    }
    return final_hash;
}