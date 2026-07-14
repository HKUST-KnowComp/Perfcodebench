#include "interface.h"
#include <cstdint>
#include <vector>

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
    if (iters <= 0) return 0;

    const uint32_t* tokens_ptr = padded_tokens.data();
    const int* lengths_ptr = lengths.data();
    uint64_t hash = 1469598103934665603ULL;
    constexpr uint64_t prime = 1099511628211ULL;

    for (int b = 0; b < batch; ++b) {
        int len = lengths_ptr[b];
        for (int i = 0; i < len; ++i) {
            uint32_t token = tokens_ptr[i];
            hash ^= token;
            hash *= prime;
            hash ^= static_cast<uint32_t>(i);
            hash *= prime;
        }
        tokens_ptr += max_len;
    }

    return hash;
}
