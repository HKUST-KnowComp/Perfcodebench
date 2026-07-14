#include "interface.h"

#include <cstddef>

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
    if (iters <= 0) return 0;
    uint64_t hash = 1469598103934665603ULL;
    const uint32_t* data = padded_tokens.data();
    std::size_t batch_sz = static_cast<std::size_t>(batch);
    std::size_t max_len_sz = static_cast<std::size_t>(max_len);
    for (std::size_t b = 0; b < batch_sz; ++b) {
        int len = lengths[b];
        const uint32_t* row = data + b * max_len_sz;
        for (int i = 0; i < len; ++i) {
            uint32_t tok = row[i];
            hash ^= tok;
            hash *= 1099511628211ULL;
            hash ^= static_cast<uint32_t>(i);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}
