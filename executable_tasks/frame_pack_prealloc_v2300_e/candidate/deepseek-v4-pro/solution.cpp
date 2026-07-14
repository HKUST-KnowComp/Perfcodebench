#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    if (iters == 0) return 0;
    uint64_t hash = 1469598103934665603ULL;
    for (const auto& frame : frames) {
        uint16_t len = static_cast<uint16_t>(frame.size());
        hash ^= static_cast<uint64_t>(len & 0xFF);
        hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>((len >> 8) & 0xFF);
        hash *= 1099511628211ULL;
        for (unsigned char c : frame) {
            hash ^= static_cast<uint64_t>(c);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}