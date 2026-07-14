#include "interface.h"

#include <cstdint>
#include <vector>
#include <immintrin.h>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters)
{
    const size_t dict_size = dictionary.size();
    const size_t row_count = codes.size();

    // Pre-compute match flags for every dictionary entry (1 = matches, 0 = doesn't)
    alignas(64) uint8_t match_flag[dict_size];
    for (size_t i = 0; i < dict_size; ++i) {
        const std::string& s = dictionary[i];
        match_flag[i] = (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm') ? 1 : 0;
    }

    uint64_t total = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t sum = 0;

        size_t i = 0;
        // Process 8 rows at a time
        for (; i + 8 <= row_count; i += 8) {
            uint64_t w0 = match_flag[codes[i+0]] * weights[i+0];
            uint64_t w1 = match_flag[codes[i+1]] * weights[i+1];
            uint64_t w2 = match_flag[codes[i+2]] * weights[i+2];
            uint64_t w3 = match_flag[codes[i+3]] * weights[i+3];
            uint64_t w4 = match_flag[codes[i+4]] * weights[i+4];
            uint64_t w5 = match_flag[codes[i+5]] * weights[i+5];
            uint64_t w6 = match_flag[codes[i+6]] * weights[i+6];
            uint64_t w7 = match_flag[codes[i+7]] * weights[i+7];
            sum += w0 + w1 + w2 + w3 + w4 + w5 + w6 + w7;
        }
        // Handle tail
        for (; i < row_count; ++i) {
            sum += match_flag[codes[i]] * weights[i];
        }
        total = sum;   // last iteration's sum is the result
    }
    return total;
}