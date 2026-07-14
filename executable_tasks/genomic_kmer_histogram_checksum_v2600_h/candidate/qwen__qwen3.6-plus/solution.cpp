#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>

namespace {

inline uint8_t char_to_bits(char c) {
    switch (c) {
        case 'A': return 0;
        case 'C': return 1;
        case 'G': return 2;
        case 'T': return 3;
        default:  return 0;
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

} // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    (void)iters;
    const std::size_t n = dna.size();
    if (k <= 0 || n < static_cast<std::size_t>(k)) {
        std::vector<uint32_t> dense(1ULL << (2 * k), 0);
        return checksum_counts(dense);
    }

    const std::size_t bins = 1ULL << (2 * k);
    std::vector<uint32_t> dense(bins, 0);
    
    const uint64_t shift = 2ULL * static_cast<uint64_t>(k);
    const uint64_t mask = (1ULL << shift) - 1;
    uint64_t code = 0;
    
    for (std::size_t i = 0; i < static_cast<std::size_t>(k) - 1; ++i) {
        code = ((code << 2) | char_to_bits(dna[i])) & mask;
    }
    
    for (std::size_t i = static_cast<std::size_t>(k) - 1; i < n; ++i) {
        code = ((code << 2) | char_to_bits(dna[i])) & mask;
        dense[code]++;
    }
    
    return checksum_counts(dense);
}
