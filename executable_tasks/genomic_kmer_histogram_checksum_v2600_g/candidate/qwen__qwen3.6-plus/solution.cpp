#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

namespace {

inline uint64_t fnv1a_checksum(const uint32_t* counts, std::size_t bins) {
    uint64_t hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < bins; ++i) {
        hash ^= static_cast<uint64_t>(counts[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

static const uint8_t kmer_lut[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0
};

inline void init_lut() {
    // A=0, C=1, G=2, T=3
    const_cast<uint8_t*>(kmer_lut)['A'] = 0;
    const_cast<uint8_t*>(kmer_lut)['C'] = 1;
    const_cast<uint8_t*>(kmer_lut)['G'] = 2;
    const_cast<uint8_t*>(kmer_lut)['T'] = 3;
}

}  // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    init_lut();
    const std::size_t bins = 1u << (2 * k);
    std::vector<uint32_t> dense(bins);
    const uint32_t mask = (1u << (2 * k)) - 1;
    const std::size_t n = dna.size();
    uint64_t hash = 0;

    if (n < static_cast<std::size_t>(k)) {
        for (int iter = 0; iter < iters; ++iter) {
            std::memset(dense.data(), 0, bins * sizeof(uint32_t));
            hash = fnv1a_checksum(dense.data(), bins);
        }
        return hash;
    }

    for (int iter = 0; iter < iters; ++iter) {
        std::memset(dense.data(), 0, bins * sizeof(uint32_t));
        
        uint32_t code = 0;
        // Build initial k-mer
        for (std::size_t i = 0; i < static_cast<std::size_t>(k); ++i) {
            code = ((code << 2) | kmer_lut[static_cast<unsigned char>(dna[i])]) & mask;
        }
        dense[code]++;

        // Rolling update
        for (std::size_t i = k; i < n; ++i) {
            code = ((code << 2) | kmer_lut[static_cast<unsigned char>(dna[i])]) & mask;
            dense[code]++;
        }

        hash = fnv1a_checksum(dense.data(), bins);
    }
    return hash;
}