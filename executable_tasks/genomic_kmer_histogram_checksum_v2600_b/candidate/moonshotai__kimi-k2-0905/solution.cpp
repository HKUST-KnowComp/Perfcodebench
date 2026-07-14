#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace {

inline uint32_t encode_char(char c) noexcept {
    // A=0, C=1, G=2, T=3
    return (c >> 1) & 0x3u;
}

uint64_t checksum_counts(const uint32_t* counts, std::size_t bins) {
    const uint64_t fnv_offset = 1469598103934665603ULL;
    const uint64_t fnv_prime  = 1099511628211ULL;
    uint64_t hash = fnv_offset;
    for (std::size_t i = 0; i < bins; ++i) {
        hash ^= static_cast<uint64_t>(counts[i]);
        hash *= fnv_prime;
    }
    return hash;
}

} // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const std::size_t bins = 1u << (2 * k);
    std::vector<uint32_t> dense(bins);
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        std::memset(dense.data(), 0, bins * sizeof(uint32_t));

        if (dna.size() < static_cast<std::size_t>(k)) {
            final_hash = checksum_counts(dense.data(), bins);
            continue;
        }

        // Build initial k-mer
        uint32_t key = 0;
        for (int i = 0; i < k; ++i) {
            key = (key << 2) | encode_char(dna[i]);
        }
        ++dense[key];

        const std::size_t mask = bins - 1;
        const int shift = 2 * k;
        for (std::size_t i = k; i < dna.size(); ++i) {
            key = ((key << 2) | encode_char(dna[i])) & mask;
            ++dense[key];
        }

        final_hash = checksum_counts(dense.data(), bins);
    }
    return final_hash;
}