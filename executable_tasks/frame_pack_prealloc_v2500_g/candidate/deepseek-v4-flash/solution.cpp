#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

constexpr uint64_t kFnvOffsetBasis = 1469598103934665603ULL;
constexpr uint64_t kFnvPrime = 1099511628211ULL;

inline void fnv_update(uint64_t& hash, uint8_t byte) {
    hash ^= static_cast<uint64_t>(byte);
    hash *= kFnvPrime;
}

} // anonymous namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    uint64_t final_hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t hash = kFnvOffsetBasis;
        for (const std::string& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            fnv_update(hash, static_cast<uint8_t>(len & 0xFF));
            fnv_update(hash, static_cast<uint8_t>((len >> 8) & 0xFF));
            for (char c : frame) {
                fnv_update(hash, static_cast<uint8_t>(c));
            }
        }
        final_hash = hash;
    }
    return final_hash;
}
