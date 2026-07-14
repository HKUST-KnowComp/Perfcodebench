#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
    if (iters <= 0) return 0;

    // Calculate total number of packed tokens
    size_t total = 0;
    for (int b = 0; b < batch; ++b) {
        total += static_cast<size_t>(lengths[b]);
    }

    // Preallocate output arrays
    std::vector<uint32_t> packed_tokens(total);
    std::vector<uint32_t> positions(total);

    // Fill packed tokens and positions in one pass
    size_t idx = 0;
    for (int b = 0; b < batch; ++b) {
        int len = lengths[b];
        const uint32_t* row = padded_tokens.data() + static_cast<size_t>(b) * static_cast<size_t>(max_len);
        for (int i = 0; i < len; ++i) {
            packed_tokens[idx] = row[i];
            positions[idx] = static_cast<uint32_t>(i);
            ++idx;
        }
    }

    // Hash computation, repeated iters times
    uint64_t hash = 0;
    const uint64_t initial_hash = 1469598103934665603ULL;
    for (int iter = 0; iter < iters; ++iter) {
        hash = initial_hash;
        for (size_t i = 0; i < total; ++i) {
            hash ^= packed_tokens[i];
            hash *= 1099511628211ULL;
            hash ^= positions[i];
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}
