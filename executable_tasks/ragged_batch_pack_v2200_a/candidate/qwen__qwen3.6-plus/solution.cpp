#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
    (void)iters;
    uint64_t hash = 1469598103934665603ULL;
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;
    
    const uint32_t* tokens = padded_tokens.data();
    const int* lens = lengths.data();
    size_t offset = 0;
    size_t ml = static_cast<size_t>(max_len);

    for (int b = 0; b < batch; ++b) {
        int len = lens[b];
        for (int i = 0; i < len; ++i) {
            hash ^= tokens[offset + i];
            hash *= FNV_PRIME;
            hash ^= static_cast<uint32_t>(i);
            hash *= FNV_PRIME;
        }
        offset += ml;
    }
    return hash;
}
