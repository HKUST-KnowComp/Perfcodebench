#include "interface.h"

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
    uint64_t hash = 0;
    const uint32_t* tokens = padded_tokens.data();
    const int* lens = lengths.data();
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (int b = 0; b < batch; ++b) {
            int len = lens[b];
            const uint32_t* row = tokens + static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len);
            for (int i = 0; i < len; ++i) {
                uint64_t token_val = static_cast<uint64_t>(row[i]);
                hash ^= token_val;
                hash *= 1099511628211ULL;
                uint64_t pos_val = static_cast<uint64_t>(static_cast<uint32_t>(i));
                hash ^= pos_val;
                hash *= 1099511628211ULL;
            }
        }
    }
    return hash;
}