#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    if (iters <= 0) return 0ULL;

    // compute total packed size
    size_t total = 0;
    for (const auto& frame : frames) {
        total += 2 + frame.size();
    }

    // pack once into preallocated buffer
    std::vector<uint8_t> out(total);
    uint8_t* ptr = out.data();
    for (const auto& frame : frames) {
        uint16_t len = static_cast<uint16_t>(frame.size());
        ptr[0] = static_cast<uint8_t>(len);
        ptr[1] = static_cast<uint8_t>(len >> 8);
        std::memcpy(ptr + 2, frame.data(), len);
        ptr += 2 + len;
    }

    // compute checksum
    uint64_t hash = 1469598103934665603ULL;
    for (uint8_t b : out) {
        hash ^= static_cast<uint64_t>(b);
        hash *= 1099511628211ULL;
    }
    return hash;
}
