#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <cstddef>

namespace {

// Lookup table for fast character encoding: A->0, C->1, G->2, T->3.
// Only uppercase needed, but lowercase included for robustness.
static const uint32_t char_to_bits[256] = {
    ['A']=0, ['C']=1, ['G']=2, ['T']=3,
    ['a']=0, ['c']=1, ['g']=2, ['t']=3
};

// FNV-1a hash over the count vector.
uint64_t checksum_counts(const std::vector<uint32_t>& counts) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t count : counts) {
        hash ^= static_cast<uint64_t>(count);
        hash *= 1099511628211ULL;
    }
    return hash;
}

// Build dense histogram using sliding window.
void build_dense_histogram(const std::string& dna, int k, std::vector<uint32_t>& dense) {
    const size_t n = dna.size();
    if (n < static_cast<size_t>(k)) return;
    const size_t mask = (1ULL << (2 * k)) - 1;
    uint32_t code = 0;
    // Encode first k-mer
    for (size_t i = 0; i < static_cast<size_t>(k); ++i) {
        code = (code << 2) | char_to_bits[static_cast<unsigned char>(dna[i])];
    }
    dense[code]++;
    // Slide window
    for (size_t i = static_cast<size_t>(k); i < n; ++i) {
        code = ((code << 2) & mask) | char_to_bits[static_cast<unsigned char>(dna[i])];
        dense[code]++;
    }
}

}  // anonymous namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const size_t bins = 1ULL << (2 * k);
    std::vector<uint32_t> dense(bins, 0);
    build_dense_histogram(dna, k, dense);
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = checksum_counts(dense);
    }
    return hash;
}