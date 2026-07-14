#include "interface.h"

#include <cstdint>
#include <vector>
#include <cstring>

namespace {

inline bool matches_prefix(const char* s) noexcept {
    return (s[0] == 'q') & (static_cast<unsigned char>(s[1]) <= static_cast<unsigned char>('m'));
}

} // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters)
{
    const std::size_t dict_size = dictionary.size();
    const std::size_t n_rows    = codes.size();

    // Pre-compute a bitset: 1 if dictionary[i] matches the prefix.
    alignas(64) uint8_t mask[8192] = {0};  // 8 KB, fits in L1
    for (std::size_t i = 0; i < dict_size; ++i) {
        const std::string& s = dictionary[i];
        if (s.size() >= 2) {
            mask[i] = matches_prefix(s.data());
        }
    }

    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;
        const uint16_t* __restrict code_ptr = codes.data();
        const uint32_t* __restrict w_ptr  = weights.data();

        std::size_t i = 0;
        // Process 8 at a time
        for (; i + 8 <= n_rows; i += 8) {
            uint64_t m0 = mask[code_ptr[i+0]];
            uint64_t m1 = mask[code_ptr[i+1]];
            uint64_t m2 = mask[code_ptr[i+2]];
            uint64_t m3 = mask[code_ptr[i+3]];
            uint64_t m4 = mask[code_ptr[i+4]];
            uint64_t m5 = mask[code_ptr[i+5]];
            uint64_t m6 = mask[code_ptr[i+6]];
            uint64_t m7 = mask[code_ptr[i+7]];

            sum += m0 * w_ptr[i+0];
            sum += m1 * w_ptr[i+1];
            sum += m2 * w_ptr[i+2];
            sum += m3 * w_ptr[i+3];
            sum += m4 * w_ptr[i+4];
            sum += m5 * w_ptr[i+5];
            sum += m6 * w_ptr[i+6];
            sum += m7 * w_ptr[i+7];
        }
        // Tail
        for (; i < n_rows; ++i) {
            sum += mask[code_ptr[i]] * w_ptr[i];
        }
        total = sum;
    }
    return total;
}