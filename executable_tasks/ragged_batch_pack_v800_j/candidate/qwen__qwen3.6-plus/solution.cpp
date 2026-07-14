#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
    const uint32_t* tokens_ptr = padded_tokens.data();
    const int* lengths_ptr = lengths.data();
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    
    uint64_t hash = 0;
    
    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        const uint32_t* row_ptr = tokens_ptr;
        for (int b = 0; b < batch; ++b) {
            int len = lengths_ptr[b];
            for (int i = 0; i < len; ++i) {
                hash ^= row_ptr[i];
                hash *= FNV_PRIME;
                hash ^= static_cast<uint64_t>(i);
                hash *= FNV_PRIME;
            }
            row_ptr += max_len;
        }
    }
    return hash;
}