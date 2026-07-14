#include "interface.h"

#include <vector>

namespace {
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;
}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    uint64_t hash = FNV_OFFSET;
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (const auto& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            hash = (hash ^ (len & 0xFF)) * FNV_PRIME;
            hash = (hash ^ (len >> 8)) * FNV_PRIME;

            const uint8_t* p = reinterpret_cast<const uint8_t*>(frame.data());
            const uint8_t* end = p + frame.size();
            while (p != end) {
                hash = (hash ^ *p++) * FNV_PRIME;
            }
        }
    }
    return hash;
}