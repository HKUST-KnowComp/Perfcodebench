#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>

namespace {

// Precomputed base encoding: A=0, C=1, G=2, T=3 (others -> 0)
static const uint8_t base_code[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,1,0,0,0,2,0,0,0,0,0,0,0,0, // 'A'=0, 'C'=1, 'G'=2
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 'T'=3 (position 84)
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

// FNV-1a offsets and primes
constexpr uint64_t fnv_offset = 1469598103934665603ULL;
constexpr uint64_t fnv_prime = 1099511628211ULL;

uint64_t checksum_counts(const std::vector<uint32_t>& counts) {
    uint64_t hash = fnv_offset;
    for (uint32_t count : counts) {
        hash ^= static_cast<uint64_t>(count);
        hash *= fnv_prime;
    }
    return hash;
}

} // anonymous namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const std::size_t bins = static_cast<std::size_t>(1) << (2 * k);
    const std::size_t n = dna.size();
    const std::size_t k_len = static_cast<std::size_t>(k);
    std::vector<uint32_t> dense(bins, 0u);

    // Build histogram once
    if (n >= k_len) {
        const uint32_t mask = static_cast<uint32_t>(bins - 1);
        uint32_t code = 0u;
        // Initial k-mer
        for (std::size_t i = 0; i < k_len; ++i) {
            code = (code << 2) | base_code[static_cast<unsigned char>(dna[i])];
        }
        dense[code]++;
        // Sliding window
        for (std::size_t i = k_len; i < n; ++i) {
            code = ((code << 2) & mask) | base_code[static_cast<unsigned char>(dna[i])];
            dense[code]++;
        }
    }

    // Compute checksum (reuse dense for all iterations)
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = checksum_counts(dense);
    }
    return hash;
}
