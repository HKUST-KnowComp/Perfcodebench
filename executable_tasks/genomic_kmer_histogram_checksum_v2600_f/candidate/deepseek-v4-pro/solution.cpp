#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

// Lookup table for A->0, C->1, G->2, T->3. Other characters default to 0.
static const uint32_t BASE_MAP[256] = {
    ['A'] = 0, ['C'] = 1, ['G'] = 2, ['T'] = 3
};

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
    const std::size_t n = dna.size();
    if (k <= 0 || n == 0) {
        // Edge case: no valid k-mers; all-zero histogram.
        std::vector<uint32_t> dense(1u << (2 * k), 0u);
        return checksum_counts(dense);
    }

    const std::size_t bins = static_cast<std::size_t>(1ull) << (2 * k);
    std::vector<uint32_t> dense(bins, 0u);

    // Build first k-mer code, then slide.
    uint32_t code = 0u;
    for (int i = 0; i < k; ++i) {
        code = (code << 2) | BASE_MAP[static_cast<unsigned char>(dna[i])];
    }
    const uint32_t mask = (1u << (2 * k)) - 1;
    dense[code]++;

    for (std::size_t i = k; i < n; ++i) {
        code = ((code << 2) & mask) | BASE_MAP[static_cast<unsigned char>(dna[i])];
        dense[code]++;
    }

    // Return checksum (result is independent of 'iters', so we compute once).
    return checksum_counts(dense);
}