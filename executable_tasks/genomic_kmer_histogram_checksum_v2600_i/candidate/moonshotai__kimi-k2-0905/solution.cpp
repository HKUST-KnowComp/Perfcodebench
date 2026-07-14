#include "interface.h"

#include <cstdint>
#include <string>
#include <cstring>

namespace {

inline uint32_t encode_char(char c) noexcept {
    return (c >> 1) & 3;       // A->0, C->1, G->2, T->3
}

uint64_t checksum_counts(const uint32_t* counts, std::size_t bins) {
    uint64_t hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < bins; ++i) {
        hash ^= static_cast<uint64_t>(counts[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const std::size_t bins = 1u << (2 * k);
    alignas(64) uint32_t dense[256] = {0};  // 4^4 = 256

    const std::size_t len = dna.size();
    if (k > 4 || len < static_cast<std::size_t>(k)) {
        // fallback for correctness, but k<=4 is the tiny-k contract
        return 0;
    }

    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        // reset histogram
        memset(dense, 0, bins * sizeof(uint32_t));

        // rolling k-mer encoding
        uint32_t key = 0;
        const char* s = dna.data();

        // prime the window
        for (int i = 0; i < k - 1; ++i) {
            key = (key << 2) | encode_char(s[i]);
        }

        const std::size_t end = len - (k - 1);
        for (std::size_t i = k - 1; i < end; ++i) {
            key = ((key << 2) | encode_char(s[i])) & (bins - 1);
            ++dense[key];
        }

        hash = checksum_counts(dense, bins);
    }
    return hash;
}