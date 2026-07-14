#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace {

inline uint64_t checksum_bytes(const uint8_t* __restrict data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* end8 = data + (len & ~7ULL);
    const uint8_t* p = data;

    while (p != end8) {
        uint64_t chunk;
        std::memcpy(&chunk, p, 8);
        p += 8;
        const uint8_t* c = reinterpret_cast<const uint8_t*>(&chunk);
        hash ^= static_cast<uint64_t>(c[0]); hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(c[1]); hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(c[2]); hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(c[3]); hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(c[4]); hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(c[5]); hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(c[6]); hash *= 1099511628211ULL;
        hash ^= static_cast<uint64_t>(c[7]); hash *= 1099511628211ULL;
    }
    while (p != data + len) {
        hash ^= static_cast<uint64_t>(*p++);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    size_t total = 0;
    for (const std::string& f : frames) {
        total += 2 + f.size();
    }

    std::vector<uint8_t> out;
    out.reserve(total);

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        out.clear();
        uint8_t* dst = out.data();
        size_t pos = 0;
        for (const std::string& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            dst[pos++] = static_cast<uint8_t>(len);
            dst[pos++] = static_cast<uint8_t>(len >> 8);
            std::memcpy(dst + pos, frame.data(), len);
            pos += len;
        }
        hash = checksum_bytes(out.data(), pos);
    }
    return hash;
}