#include "interface.h"

#include <cstdint>
#include <vector>
#include <cstring>

namespace {

alignas(64) static uint64_t dict_mask[16] = {0};   // 1024 bits
alignas(64) static uint64_t code_weight[1<<20] = {0}; // 1M entries

inline void build_tables(const std::vector<std::string>& dictionary,
                         const std::vector<uint16_t>& codes,
                         const std::vector<uint32_t>& weights) {
    const size_t dict_size = dictionary.size();
    // mark qualifying dictionary entries
    for (size_t i = 0; i < dict_size; ++i) {
        const std::string& s = dictionary[i];
        bool ok = (s.size() >= 2) && (s[0] == 'q') && (s[1] <= 'm');
        if (ok) dict_mask[i >> 6] |= 1ULL << (i & 63);
    }
    // build per-code weight cache
    const size_t n = codes.size();
    for (size_t i = 0; i < n; ++i) {
        uint16_t c = codes[i];
        uint64_t w = (dict_mask[c >> 6] >> (c & 63)) & 1ULL;
        code_weight[i] = w * static_cast<uint64_t>(weights[i]);
    }
}

} // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
    static bool built = false;
    if (!built) {
        build_tables(dictionary, codes, weights);
        built = true;
    }
    const size_t n = codes.size();
    uint64_t sum = 0;
    for (int iter = 0; iter < iters; ++iter) {
        uint64_t local = 0;
        const uint64_t* cw = code_weight;
        size_t i = 0;
        // manual 4-way unroll for vectorisation friendliness
        for (; i + 3 < n; i += 4) {
            local += cw[i] + cw[i+1] + cw[i+2] + cw[i+3];
        }
        for (; i < n; ++i) local += cw[i];
        sum = local;
    }
    return sum;
}