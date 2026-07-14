#include "interface.h"
#include <cstring>
#include <vector>

namespace {

inline uint64_t checksum_bytes(const uint8_t* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    const uint8_t* end8 = data + (len & ~size_t(7));
    const uint8_t* p = data;

    // 8-byte blocks
    while (p != end8) {
        uint64_t v;
        std::memcpy(&v, p, 8);
        for (int i = 0; i < 8; ++i) {
            hash ^= (v & 0xFFULL);
            v >>= 8;
            hash *= prime;
        }
        p += 8;
    }
    // tail
    for (; p < data + len; ++p) {
        hash ^= *p;
        hash *= prime;
    }
    return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    size_t total = 0;
    for (const auto& f : frames) total += 2 + f.size();

    std::vector<uint8_t> out;
    out.reserve(total);

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        out.resize(total);
        uint8_t* dst = out.data();

        for (const auto& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            dst[0] = static_cast<uint8_t>(len);
            dst[1] = static_cast<uint8_t>(len >> 8);
            dst += 2;
            std::memcpy(dst, frame.data(), len);
            dst += len;
        }
        hash = checksum_bytes(out.data(), total);
    }
    return hash;
}