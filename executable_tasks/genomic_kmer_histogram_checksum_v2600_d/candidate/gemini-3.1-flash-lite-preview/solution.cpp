#include "interface.h"
#include <vector>
#include <cstdint>
#include <string>
#include <algorithm>

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const size_t n = dna.size();
    if (n < static_cast<size_t>(k)) return 0;

    const size_t bins = 1ULL << (2 * k);
    const uint32_t mask = (1U << (2 * k)) - 1;
    std::vector<uint32_t> dense(bins);

    uint64_t final_hash = 0;

    static const uint8_t char_map[256] = {
        ['A'] = 0, ['C'] = 1, ['G'] = 2, ['T'] = 3
    };

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(dense.begin(), dense.end(), 0u);

        uint32_t current_kmer = 0;
        for (int i = 0; i < k; ++i) {
            current_kmer = (current_kmer << 2) | char_map[static_cast<uint8_t>(dna[i])];
        }
        dense[current_kmer]++;

        for (size_t i = k; i < n; ++i) {
            current_kmer = ((current_kmer << 2) | char_map[static_cast<uint8_t>(dna[i])]) & mask;
            dense[current_kmer]++;
        }

        uint64_t hash = 1469598103934665603ULL;
        for (uint32_t count : dense) {
            hash ^= static_cast<uint64_t>(count);
            hash *= 1099511628211ULL;
        }
        final_hash = hash;
    }

    return final_hash;
}