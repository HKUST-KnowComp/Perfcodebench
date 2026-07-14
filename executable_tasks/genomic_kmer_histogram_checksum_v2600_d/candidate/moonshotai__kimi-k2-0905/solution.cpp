#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

namespace {

inline uint32_t char2bits(char c) noexcept {
    return (c >> 1) & 3u;          // A=0, C=1, G=2, T=3
}

inline uint64_t checksum_counts(const uint32_t* __restrict counts, std::size_t n) noexcept {
    uint64_t h = 1469598103934665603ULL;
    for (std::size_t i = 0; i < n; ++i) {
        h ^= static_cast<uint64_t>(counts[i]);
        h *= 1099511628211ULL;
    }
    return h;
}

} // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const std::size_t L = dna.size();
    const std::size_t K = static_cast<std::size_t>(k);
    if (K == 0 || K > 16 || L < K) return 0;

    const std::size_t bins = 1u << (2 * K);
    std::vector<uint32_t> dense(bins, 0u);
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        std::memset(dense.data(), 0, bins * sizeof(uint32_t));

        uint32_t key = 0;
        const char* s = dna.data();

        // seed first k-1 characters
        for (std::size_t i = 0; i < K - 1; ++i) {
            key = (key << 2) | char2bits(s[i]);
        }

        const std::size_t shift = 2 * (K - 1);
        const uint32_t mask = (1u << (2 * K)) - 1u;

        for (std::size_t i = K - 1; i < L; ++i) {
            key = ((key << 2) | char2bits(s[i])) & mask;
            dense[key]++;
        }

        final_hash = checksum_counts(dense.data(), bins);
    }
    return final_hash;
}