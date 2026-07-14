#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

namespace {

inline uint32_t encode_base(char c) noexcept {
    // A=0, C=1, G=2, T=3
    return (c >> 1) & 3u;
}

uint64_t checksum_counts(const uint32_t* counts, std::size_t n) noexcept {
    uint64_t hash = 14695981039346656037ULL;
    const uint64_t prime = 1099511628211ULL;
    for (std::size_t i = 0; i < n; ++i) {
        hash ^= static_cast<uint64_t>(counts[i]);
        hash *= prime;
    }
    return hash;
}

} // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const std::size_t bins = 1u << (2 * k);
    std::vector<uint32_t> dense(bins, 0u);
    uint64_t final_hash = 0;

    const std::size_t len = dna.size();
    const std::size_t mask = bins - 1u;

    for (int iter = 0; iter < iters; ++iter) {
        std::memset(dense.data(), 0, bins * sizeof(uint32_t));

        uint32_t code = 0;
        std::size_t i = 0;

        // warm-up: fill the first k-1 bases
        for (; i + 1 < static_cast<std::size_t>(k); ++i) {
            code = (code << 2) | encode_base(dna[i]);
        }

        // roll through the rest
        for (; i < len; ++i) {
            code = ((code << 2) | encode_base(dna[i])) & mask;
            dense[code]++;
        }

        final_hash = checksum_counts(dense.data(), bins);
    }
    return final_hash;
}