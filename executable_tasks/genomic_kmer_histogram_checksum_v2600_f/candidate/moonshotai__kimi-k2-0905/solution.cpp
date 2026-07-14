#include "interface.h"
#include <algorithm>
#include <cstdint>
#include <string>
#include <vector>

namespace {

inline uint32_t encode_base(char c) noexcept {
    return (c >> 1) & 3u;          // A=0, C=1, G=2, T=3
}

inline uint64_t checksum_counts(const uint32_t* __restrict data,
                                std::size_t size) noexcept {
    uint64_t hash = 14695981039346656037ULL;
    const uint64_t prime = 1099511628211ULL;
    for (std::size_t i = 0; i < size; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= prime;
    }
    return hash;
}

} // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const std::size_t mask = (1u << (2 * k)) - 1u;
    const std::size_t len = dna.size();
    const std::size_t last = (len < static_cast<std::size_t>(k)) ? 0 : len - k + 1;

    std::vector<uint32_t> dense(mask + 1, 0u);

    uint64_t final_hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        std::fill(dense.begin(), dense.end(), 0u);

        uint32_t curr = 0;
        std::size_t i = 0;
        for (; i < static_cast<std::size_t>(k - 1) && i < len; ++i)
            curr = ((curr << 2) | encode_base(dna[i])) & mask;

        for (; i < len; ++i) {
            curr = ((curr << 2) | encode_base(dna[i])) & mask;
            ++dense[curr];
        }
        final_hash = checksum_counts(dense.data(), mask + 1);
    }
    return final_hash;
}