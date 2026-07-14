#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline uint8_t get_kmer_char_value(char c) {
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
    uint32_t mask = (1u << (2 * k)) - 1;

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(dense.begin(), dense.end(), 0u);

        if (dna.size() < static_cast<std::size_t>(k)) {
            continue;
        }

        uint32_t code = 0;
        for (int j = 0; j < k; ++j) {
            code = (code << 2) | get_kmer_char_value(dna[j]);
        }
        dense[code]++;

        for (std::size_t i = 1; i + static_cast<std::size_t>(k) <= dna.size(); ++i) {
            code = ((code << 2) | get_kmer_char_value(dna[i + k - 1])) & mask;
            dense[code]++;
        }

        hash = checksum_counts(dense);
    }
    return hash;
}