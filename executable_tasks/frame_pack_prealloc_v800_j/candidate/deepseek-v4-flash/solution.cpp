#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>

namespace {
    uint64_t fnv1a(const uint8_t* data, size_t len) {
        uint64_t hash = 1469598103934665603ULL;
        for (size_t i = 0; i < len; ++i) {
            hash ^= static_cast<uint64_t>(data[i]);
            hash *= 1099511628211ULL;
        }
        return hash;
    }
}

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
    if (iters == 0) return 0;

    // Precompute total size
    size_t total = 0;
    for (const auto& f : frames) {
        total += 2 + f.size();
    }

    // Build packed buffer once
    std::vector<uint8_t> out(total);
    size_t pos = 0;
    for (const auto& f : frames) {
        uint16_t len = static_cast<uint16_t>(f.size());
        // little-endian length prefix
        out[pos++] = static_cast<uint8_t>(len & 0xFF);
        out[pos++] = static_cast<uint8_t>((len >> 8) & 0xFF);
        // copy frame content
        for (char c : f) {
            out[pos++] = static_cast<uint8_t>(c);
        }
    }

    // Single checksum computation (identical for all iterations)
    return fnv1a(out.data(), out.size());
}
