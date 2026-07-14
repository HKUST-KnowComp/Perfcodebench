#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens,
                              const std::vector<int>& lengths,
                              int batch,
                              int max_len,
                              int iters) {
    const uint32_t* tokens_ptr = padded_tokens.data();
    const int* lengths_ptr = lengths.data();
    const size_t max_len_s = static_cast<size_t>(max_len);
    
    constexpr uint64_t kFnvOffset = 1469598103934665603ULL;
    constexpr uint64_t kFnvPrime  = 1099511628211ULL;
    
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = kFnvOffset;
        for (int b = 0; b < batch; ++b) {
            const size_t row_offset = static_cast<size_t>(b) * max_len_s;
            const int len = lengths_ptr[b];
            for (int i = 0; i < len; ++i) {
                const size_t idx = row_offset + static_cast<size_t>(i);
                uint64_t token = tokens_ptr[idx];
                hash ^= token;
                hash *= kFnvPrime;
                uint64_t pos = static_cast<uint32_t>(i);
                hash ^= pos;
                hash *= kFnvPrime;
            }
        }
    }
    return hash;
}
