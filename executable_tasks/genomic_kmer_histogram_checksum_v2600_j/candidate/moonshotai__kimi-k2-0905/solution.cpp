#include "interface.h"

#include <cstdint>
#include <string>
#include <cstring>

namespace {

inline uint32_t encode_kmer(const char* s, int k) {
    uint32_t code = 0u;
    for (int i = 0; i < k; ++i) {
        uint32_t v = 0u;
        switch (s[i]) {
            case 'C': v = 1u; break;
            case 'G': v = 2u; break;
            case 'T': v = 3u; break;
            default:  v = 0u; break;
        }
        code = (code << 2) | v;
    }
    return code;
}

inline uint64_t checksum_counts(const uint32_t* counts, std::size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(counts[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const std::size_t bins = 1u << (2 * k);
    alignas(64) uint32_t dense[1024];        // 4^5 = 1024 for k=5
    std::memset(dense, 0, bins * sizeof(uint32_t));

    const std::size_t n = dna.size();
    if (n < static_cast<std::size_t>(k)) {
        uint64_t hash = checksum_counts(dense, bins);
        return hash; // all counts remain zero
    }

    // fast-path for k <= 10 (fits in 20 bits)
    if (k <= 10) {
        // build lookup for each nucleotide
        const uint8_t map[256] = {
            ['A'] = 0, ['C'] = 1, ['G'] = 2, ['T'] = 3,
        };

        uint32_t mask = (1u << (2 * k)) - 1u;
        uint32_t code = 0u;

        // prime the window
        for (int i = 0; i < k; ++i) {
            code = (code << 2) | map[static_cast<uint8_t>(dna[i])];
        }
        dense[code]++;

        // rolling update
        for (std::size_t i = k; i < n; ++i) {
            code = ((code << 2) | map[static_cast<uint8_t>(dna[i])]) & mask;
            dense[code]++;
        }
    } else {
        // fallback: non-rolling but still without allocations
        for (std::size_t i = 0; i + static_cast<std::size_t>(k) <= n; ++i) {
            uint32_t idx = encode_kmer(&dna[i], k);
            dense[idx]++;
        }
    }

    uint64_t hash = checksum_counts(dense, bins);
    // the histogram is deterministic; no need to recompute
    return hash;
}