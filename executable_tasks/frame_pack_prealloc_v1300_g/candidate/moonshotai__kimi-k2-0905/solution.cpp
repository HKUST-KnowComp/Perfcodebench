#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>

namespace {

inline uint64_t checksum_bytes(const uint8_t* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    size_t total = 0;
    for (const auto& f : frames) total += 2 + f.size();

    std::vector<uint8_t> out;
    out.reserve(total);

    for (int iter = 0; iter < iters; ++iter) {
        out.clear();
        for (const auto& frame : frames) {
            uint16_t len = static_cast<uint16_t>(frame.size());
            out.push_back(static_cast<uint8_t>(len));
            out.push_back(static_cast<uint8_t>(len >> 8));
            out.insert(out.end(), frame.data(), frame.data() + frame.size());
        }
    }
    return checksum_bytes(out.data(), out.size());
}