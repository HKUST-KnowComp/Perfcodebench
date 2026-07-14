#include "interface.h"

#include <cstdint>
#include <vector>
#include <cstring>

namespace {

alignas(64) static thread_local uint64_t dict_mask[16];   // 1024-bit bitset

inline bool matches_prefix(const std::string& s) noexcept {
    return s.size() >= 2 && s[0] == 'q' && s[1] <= 'm';
}

} // anonymous namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters)
{
    const std::size_t dict_size = dictionary.size();
    const std::size_t row_count = codes.size();

    // ----- 1.  one-time setup: mark qualifying dictionary entries -----
    std::memset(dict_mask, 0, sizeof(dict_mask));
    for (std::size_t i = 0; i < dict_size; ++i) {
        if (matches_prefix(dictionary[i])) {
            const std::size_t word_idx = i;
            const std::size_t block     = word_idx >> 6;
            const std::size_t bit       = word_idx & 63;
            dict_mask[block] |= (1ULL << bit);
        }
    }

    // ----- 2.  hot loop: only sum rows whose code maps to a marked entry -----
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local_sum = 0;
        const uint16_t* __restrict code_ptr  = codes.data();
        const uint32_t* __restrict weight_ptr = weights.data();

        std::size_t i = 0;
        // manual 8-way unroll to hide latency and help auto-vectorisation
        for (; i + 8 <= row_count; i += 8) {
            for (std::size_t k = 0; k < 8; ++k) {
                const uint16_t c = code_ptr[i + k];
                const uint64_t block = dict_mask[c >> 6];
                const uint64_t bit   = 1ULL << (c & 63);
                if (block & bit) {
                    local_sum += static_cast<uint64_t>(weight_ptr[i + k]);
                }
            }
        }
        // tail
        for (; i < row_count; ++i) {
            const uint16_t c = code_ptr[i];
            const uint64_t block = dict_mask[c >> 6];
            const uint64_t bit   = 1ULL << (c & 63);
            if (block & bit) {
                local_sum += static_cast<uint64_t>(weight_ptr[i]);
            }
        }
        sum = local_sum;   // store back so the loop is not optimised away
    }
    return sum;
}