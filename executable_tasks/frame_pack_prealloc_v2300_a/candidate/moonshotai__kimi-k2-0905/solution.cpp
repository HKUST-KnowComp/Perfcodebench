#include "interface.h"

#include <vector>
#include <cstring>

namespace {

inline uint64_t checksum_bytes(const uint8_t* __restrict data, size_t len) {
    const uint64_t prime = 1099511628211ULL;
    uint64_t hash = 1469598103934665603ULL;

    size_t i = 0;
    for (; i + 8 <= len; i += 8) {
        uint64_t chunk;
        std::memcpy(&chunk, data + i, 8);
        hash ^= chunk;
        hash *= prime;
    }
    for (; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= prime;
    }
    return hash;
}

inline void write_len(std::vector<uint8_t>& out, uint16_t v) {
    uint8_t buf[2];
    buf[0] = static_cast<uint8_t>(v);
    buf[1] = static_cast<uint8_t>(v >> 8);
    out.insert(out.end(), buf, buf + 2);
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    size_t total = frames.size() * 2;
    for (const auto& f : frames) total += f.size();

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