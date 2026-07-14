#include "interface.h"
#include <vector>
#include <cstdint>

namespace {
inline uint64_t mix(uint64_t h, uint64_t v) {
    h ^= v;
    h *= 1099511628211ULL;
    return h;
}
} // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens,
                              const std::vector<int>& lengths,
                              int batch, int max_len, int iters)
{
    // one-time capacity reservation
    std::vector<uint32_t> packed_tokens;
    std::vector<uint32_t> positions;
    size_t total = 0;
    for (int b = 0; b < batch; ++b) total += static_cast<size_t>(lengths[static_cast<size_t>(b)]);
    packed_tokens.reserve(total);
    positions.reserve(total);

    uint64_t hash = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
        packed_tokens.clear();
        positions.clear();

        const uint32_t* row = padded_tokens.data();
        for (int b = 0; b < batch; ++b) {
            int len = lengths[static_cast<size_t>(b)];
            for (int i = 0; i < len; ++i) {
                packed_tokens.push_back(row[i]);
                positions.push_back(static_cast<uint32_t>(i));
            }
            row += max_len;
        }

        // fold hash update into the same iteration
        const size_t n = packed_tokens.size();
        for (size_t i = 0; i < n; ++i) {
            hash = mix(hash, packed_tokens[i]);
            hash = mix(hash, positions[i]);
        }
    }
    return hash;
}