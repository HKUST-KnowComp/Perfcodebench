#include "interface.h"
#include <vector>
#include <numeric>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}

} // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
    if (iters <= 0) return 0;

    // Pre-calculate total size to avoid reallocations
    size_t total_elements = 0;
    for (int b = 0; b < batch; ++b) {
        total_elements += lengths[static_cast<size_t>(b)];
    }

    std::vector<uint32_t> packed_tokens;
    std::vector<uint32_t> positions;
    packed_tokens.reserve(total_elements);
    positions.reserve(total_elements);

    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        packed_tokens.clear();
        positions.clear();

        // Pack tokens and positions
        for (int b = 0; b < batch; ++b) {
            int len = lengths[static_cast<size_t>(b)];
            size_t offset = static_cast<size_t>(b) * max_len;
            for (int i = 0; i < len; ++i) {
                packed_tokens.push_back(padded_tokens[offset + i]);
                positions.push_back(static_cast<uint32_t>(i));
            }
        }

        // Compute checksum
        hash = 1469598103934665603ULL;
        const uint32_t* p_tokens = packed_tokens.data();
        const uint32_t* p_pos = positions.data();
        for (size_t i = 0; i < total_elements; ++i) {
            hash = mix(hash, static_cast<uint64_t>(p_tokens[i]));
            hash = mix(hash, static_cast<uint64_t>(p_pos[i]));
        }
    }

    return hash;
}