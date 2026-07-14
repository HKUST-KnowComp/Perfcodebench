#include "interface.h"

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    if (iters <= 0) return 0;
    uint64_t hash = 1469598103934665603ULL;
    for (const std::string& frame : frames) {
        uint16_t len = static_cast<uint16_t>(frame.size());
        uint8_t b0 = static_cast<uint8_t>(len & 0xFF);
        uint8_t b1 = static_cast<uint8_t>((len >> 8) & 0xFF);
        hash ^= b0;
        hash *= 1099511628211ULL;
        hash ^= b1;
        hash *= 1099511628211ULL;
        for (const char& c : frame) {
            hash ^= static_cast<uint8_t>(c);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}