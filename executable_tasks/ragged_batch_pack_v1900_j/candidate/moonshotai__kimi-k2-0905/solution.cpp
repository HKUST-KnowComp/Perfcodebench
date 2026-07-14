#include "interface.h"
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens,
                                const std::vector<int>& lengths,
                                int batch, int max_len, int iters) {
    const size_t total = lengths.size() ? static_cast<size_t>(lengths[0]) : 0;
    std::vector<uint32_t> packed_tokens(total);
    std::vector<uint32_t> positions(total);
    
    uint64_t hash = 1469598103934665603ULL;

    for (int iter = 0; iter < iters; ++iter) {
        size_t pos = 0;
        const int* lens = lengths.data();
        const uint32_t* src = padded_tokens.data();

        for (int b = 0; b < batch; ++b) {
            const int len = lens[b];
            const size_t base = static_cast<size_t>(b) * static_cast<size_t>(max_len);
            for (int i = 0; i < len; ++i) {
                packed_tokens[pos] = src[base + static_cast<size_t>(i)];
                positions[pos] = static_cast<uint32_t>(i);
                ++pos;
            }
        }

        for (size_t i = 0; i < pos; ++i) {
            hash = mix(hash, packed_tokens[i]);
            hash = mix(hash, positions[i]);
        }
    }
    return hash;
}