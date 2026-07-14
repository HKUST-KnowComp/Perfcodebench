#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>
#include <cstddef>

namespace {

inline uint8_t base_to_val(char c) {
    switch (c) {
        case 'A': return 0;
        case 'C': return 1;
        case 'G': return 2;
        case 'T': return 3;
        default: return 0;
    }
}

uint64_t checksum_counts(const std::vector<uint32_t>& counts) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t count : counts) {
        hash ^= static_cast<uint64_t>(count);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const std::size_t bins = 1u << (2 * k);
    std::vector<uint32_t> dense(bins, 0u);
    uint64_t hash = 0;
    const std::size_t n = dna.size();
    if (n < static_cast<std::size_t>(k)) return 0;

    const uint32_t mask = (1u << (2 * k)) - 1;

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(dense.begin(), dense.end(), 0u);

        uint32_t code = 0;
        for (int i = 0; i < k; ++i) {
            code = (code << 2) | base_to_val(dna[i]);
        }
        dense[code]++;

        for (std::size_t i = k; i < n; ++i) {
            code = ((code & mask) << 2) | base_to_val(dna[i]);
            dense[code]++;
        }

        hash = checksum_counts(dense);
    }
    return hash;
}