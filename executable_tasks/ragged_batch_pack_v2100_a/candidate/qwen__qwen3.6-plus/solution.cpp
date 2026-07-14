#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
    if (iters <= 0) return 0;

    const uint32_t* tokens_ptr = padded_tokens.data();
    const int* lengths_ptr = lengths.data();
    uint64_t hash = 1469598103934665603ULL;
    size_t offset = 0;

    for (int b = 0; b < batch; ++b) {
        int len = lengths_ptr[b];
        const uint32_t* row_ptr = tokens_ptr + offset;
        for (int i = 0; i < len; ++i) {
            hash ^= row_ptr[i];
            hash *= 1099511628211ULL;
            hash ^= static_cast<uint64_t>(i);
            hash *= 1099511628211ULL;
        }
        offset += max_len;
    }
    return hash;
}