#include "interface.h"

#include <vector>
#include <cstdint>

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
    // Pre-compute total packed length
    std::size_t total = 0;
    for (int b = 0; b < batch; ++b) {
        total += static_cast<std::size_t>(lengths[static_cast<std::size_t>(b)]);
    }

    // Pre-allocate buffers once
    std::vector<uint32_t> packed_tokens(total);
    std::vector<uint32_t> positions(total);

    const uint32_t* __restrict__ src_base = padded_tokens.data();
    const int* __restrict__ len_ptr = lengths.data();
    const std::size_t ml = static_cast<std::size_t>(max_len);

    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        uint32_t* __restrict__ tok_out = packed_tokens.data();
        uint32_t* __restrict__ pos_out = positions.data();

        // Pack tokens and positions
        for (int b = 0; b < batch; ++b) {
            const int len = len_ptr[b];
            const uint32_t* __restrict__ row = src_base + static_cast<std::size_t>(b) * ml;
            for (int i = 0; i < len; ++i) {
                *tok_out++ = row[i];
                *pos_out++ = static_cast<uint32_t>(i);
            }
        }

        // Compute hash
        hash = 1469598103934665603ULL;
        const uint32_t* __restrict__ tk = packed_tokens.data();
        const uint32_t* __restrict__ ps = positions.data();
        for (std::size_t i = 0; i < total; ++i) {
            hash ^= tk[i];
            hash *= 1099511628211ULL;
            hash ^= ps[i];
            hash *= 1099511628211ULL;
        }
    }

    return hash;
}
