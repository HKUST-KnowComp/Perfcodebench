#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

namespace {
    inline uint32_t base_to_val(char c) {
        switch (c) {
            case 'A': return 0;
            case 'C': return 1;
            case 'G': return 2;
            case 'T': return 3;
            default: return 0;
        }
    }
}

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const size_t bins = 1u << (2 * k);
    std::vector<uint32_t> dense(bins, 0u);
    const uint32_t mask = bins - 1;
    const size_t n = dna.size();

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        std::memset(dense.data(), 0, bins * sizeof(uint32_t));

        if (n < static_cast<size_t>(k)) {
            hash = 1469598103934665603ULL;
            for (size_t i = 0; i < bins; ++i) {
                hash ^= 0;
                hash *= 1099511628211ULL;
            }
            continue;
        }

        uint32_t code = 0;
        for (int i = 0; i < k; ++i) {
            code = (code << 2) | base_to_val(dna[i]);
        }
        dense[code]++;

        for (size_t i = k; i < n; ++i) {
            code = ((code << 2) & mask) | base_to_val(dna[i]);
            dense[code]++;
        }

        hash = 1469598103934665603ULL;
        for (size_t i = 0; i < bins; ++i) {
            hash ^= static_cast<uint64_t>(dense[i]);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}