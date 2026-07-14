#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

namespace {

static const uint8_t dna_lut[256] = {
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
    const_cast<uint8_t&>(dna_lut['A']) = 0;
    const_cast<uint8_t&>(dna_lut['C']) = 1;
    const_cast<uint8_t&>(dna_lut['G']) = 2;
    const_cast<uint8_t&>(dna_lut['T']) = 3;
}

uint64_t checksum_counts(const uint32_t* counts, size_t n) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < n; ++i) {
        hash ^= static_cast<uint64_t>(counts[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    init_lut();
    if (k <= 0 || dna.size() < static_cast<size_t>(k)) {
        return 1469598103934665603ULL;
    }

    const size_t bins = 1ULL << (2 * k);
    std::vector<uint32_t> dense(bins);
    const size_t n = dna.size();
    const size_t limit = n - k + 1;
    const uint32_t mask = (1ULL << (2 * k)) - 1;
    const uint8_t* data = reinterpret_cast<const uint8_t*>(dna.data());

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        std::memset(dense.data(), 0, bins * sizeof(uint32_t));

        uint32_t code = 0;
        for (size_t i = 0; i < k; ++i) {
            code = (code << 2) | dna_lut[data[i]];
        }
        dense[code]++;

        for (size_t i = 1; i < limit; ++i) {
            code = ((code & mask) << 2) | dna_lut[data[i + k - 1]];
            dense[code]++;
        }

        hash = checksum_counts(dense.data(), bins);
    }
    return hash;
}
