#include "interface.h"
#include <vector>
#include <cstring>

namespace {

inline uint64_t checksum_bytes(const uint8_t* data, size_t len) {
    const uint64_t prime = 1099511628211ULL;
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* end8 = data + (len & ~size_t(7));
    const uint8_t* p = data;
    while (p != end8) {
        uint64_t chunk;
        std::memcpy(&chunk, p, 8);
        hash ^= chunk;
        hash *= prime;
        p += 8;
    }
    for (size_t i = len & ~size_t(7); i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= prime;
    }
    return hash;
}

inline void write_len(std::vector<uint8_t>& out, uint16_t v) {
    out.push_back(static_cast<uint8_t>(v));
    out.push_back(static_cast<uint8_t>(v >> 8));
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    size_t total = 0;
    for (const auto& f : frames) total += 2 + f.size();
    std::vector<uint8_t> out;
    out.reserve(total);

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        out.clear();
        for (const std::string& frame : frames) {
            write_len(out, static_cast<uint16_t>(frame.size()));
            out.insert(out.end(),
                       reinterpret_cast<const uint8_t*>(frame.data()),
                       reinterpret_cast<const uint8_t*>(frame.data() + frame.size()));
        }
        hash = checksum_bytes(out.data(), out.size());
    }
    return hash;
}