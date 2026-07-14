#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace {

alignas(64) static const uint8_t BASE_MAP[256] = {
    ['A'] = 0, ['C'] = 1, ['G'] = 2, ['T'] = 3,
    ['a'] = 0, ['c'] = 1, ['g'] = 2, ['t'] = 3
};

inline uint64_t checksum_counts(const uint32_t* counts, std::size_t bins) {
    uint64_t hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < bins; ++i) {
        hash ^= static_cast<uint64_t>(counts[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const std::size_t n = dna.size();
    const std::size_t bins = static_cast<std::size_t>(1) << (2 * k);
    const uint32_t mask = static_cast<uint32_t>(bins - 1);
    const int ku = k;

    if (n < static_cast<std::size_t>(ku)) {
        std::vector<uint32_t> dense(bins, 0u);
        return checksum_counts(dense.data(), bins);
    }

    std::vector<uint32_t> dense(bins);
    const char* data = dna.data();
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        std::memset(dense.data(), 0, bins * sizeof(uint32_t));

        // Build initial k-mer code
        uint32_t code = 0;
        for (int i = 0; i < ku; ++i) {
            code = (code << 2) | BASE_MAP[static_cast<unsigned char>(data[i])];
        }
        ++dense[code];

        // Slide window
        const std::size_t limit = n - static_cast<std::size_t>(ku);
        for (std::size_t i = 0; i < limit; ++i) {
            code = ((code << 2) | BASE_MAP[static_cast<unsigned char>(data[i + ku])]) & mask;
            ++dense[code];
        }

        hash = checksum_counts(dense.data(), bins);
    }
    return hash;
}
