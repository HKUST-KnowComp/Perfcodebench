#include "interface.h"

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
    // Compute total number of packed tokens
    std::size_t total_packed = 0;
    for (int b = 0; b < batch; ++b) {
        total_packed += static_cast<std::size_t>(lengths[b]);
    }

    // Pack once outside the iteration loop
    std::vector<uint32_t> packed_tokens(total_packed);
    std::vector<uint32_t> positions(total_packed);
    std::size_t idx = 0;
    for (int b = 0; b < batch; ++b) {
        int len = lengths[b];
        std::size_t row_offset = static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len);
        for (int i = 0; i < len; ++i) {
            packed_tokens[idx] = padded_tokens[row_offset + static_cast<std::size_t>(i)];
            positions[idx] = static_cast<uint32_t>(i);
            ++idx;
        }
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (std::size_t i = 0; i < total_packed; ++i) {
            // mix token
            hash ^= packed_tokens[i];
            hash *= 1099511628211ULL;
            // mix position
            hash ^= positions[i];
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}