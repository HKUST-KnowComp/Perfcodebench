#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

// Encode a single base character to a 2-bit value.
// For characters not A/C/G/T, map to 0 (A) to avoid errors.
// Assumes input is valid DNA (A,C,G,T).
uint32_t base_code(char c) noexcept {
    switch (c) {
        case 'C': return 1;
        case 'G': return 2;
        case 'T': return 3;
        default:  return 0; // 'A' or anything else -> 0
    }
}

// Compute FNV-1a 64-bit hash over the histogram array.
uint64_t checksum_counts(const std::vector<uint32_t>& counts) noexcept {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t count : counts) {
        hash ^= static_cast<uint64_t>(count);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // anonymous namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    // Number of possible k-mers = 4^k, fits in 2k bits.
    const std::size_t bins = static_cast<std::size_t>(1) << (2 * k);
    std::vector<uint32_t> dense(bins, 0u);
    uint64_t hash = 0;

    // Precompute the mask that keeps only the lowest 2k bits.
    const uint32_t mask = (bins != 0) ? static_cast<uint32_t>(bins - 1) : 0;

    const std::size_t len = dna.size();
    if (static_cast<std::size_t>(k) > len) {
        // No k-mers, checksum over all-zero histogram.
        for (int iter = 0; iter < iters; ++iter) {
            std::fill(dense.begin(), dense.end(), 0u);
            hash = checksum_counts(dense);
        }
        return hash;
    }

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(dense.begin(), dense.end(), 0u);

        // Initialize sliding window code for first k bases.
        uint32_t code = 0;
        for (int i = 0; i < k; ++i) {
            code = ((code << 2) | base_code(dna[static_cast<std::size_t>(i)])) & mask;
        }
        ++dense[code];

        // Slide over the remaining positions.
        for (std::size_t i = static_cast<std::size_t>(k); i < len; ++i) {
            code = ((code << 2) | base_code(dna[i])) & mask;
            ++dense[code];
        }

        hash = checksum_counts(dense);
    }
    return hash;
}
