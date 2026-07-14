#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline void fnv1a_update(uint64_t& hash, uint8_t byte) {
    hash ^= static_cast<uint64_t>(byte);
    hash *= FNV_PRIME;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET_BASIS;
        for (const std::string& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            fnv1a_update(hash, static_cast<uint8_t>(len & 0xFF));
            fnv1a_update(hash, static_cast<uint8_t>((len >> 8) & 0xFF));
            for (uint8_t c : frame) {
                fnv1a_update(hash, c);
            }
        }
    }
    return hash;
}
