#include "interface.h"

#include <cstdint>
#include <vector>
#include <string>

namespace {
    constexpr uint64_t kFnvOffsetBasis = 1469598103934665603ULL;
    constexpr uint64_t kFnvPrime = 1099511628211ULL;

    inline void fnv_update(uint64_t& hash, uint8_t byte) {
        hash ^= static_cast<uint64_t>(byte);
        hash *= kFnvPrime;
    }
} // anonymous namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = kFnvOffsetBasis;
        for (const std::string& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            // write length as little-endian two bytes
            fnv_update(hash, static_cast<uint8_t>(len & 0xff));
            fnv_update(hash, static_cast<uint8_t>((len >> 8) & 0xff));
            // write frame bytes and update hash directly
            const uint8_t* data = reinterpret_cast<const uint8_t*>(frame.data());
            size_t sz = frame.size();
            for (size_t i = 0; i < sz; ++i) {
                fnv_update(hash, data[i]);
            }
        }
    }
    return hash;
}
