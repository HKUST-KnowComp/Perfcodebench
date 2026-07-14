#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace {

inline uint32_t encode_kmer(const char* s, int k) {
    uint32_t code = 0;
    for (int i = 0; i < k; ++i) {
        uint32_t v = 0;
        switch (s[i]) {
            case 'A': v = 0; break;
            case 'C': v = 1; break;
            case 'G': v = 2; break;
            case 'T': v = 3; break;
            default: v = 0; break; // treat invalid as A
        }
        code = (code << 2) | v;
    }
    return code;
}

inline uint64_t checksum_counts(const uint32_t* counts, std::size_t bins) {
    const uint64_t fnv_offset = 1469598103934665603ULL;
    const uint64_t fnv_prime  = 1099511628211ULL;
    uint64_t hash = fnv_offset;
    for (std::size_t i = 0; i < bins; ++i) {
        hash ^= static_cast<uint64_t>(counts[i]);
        hash *= fnv_prime;
    }
    return hash;
}

} // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const std::size_t bins = 1u << (2 * k);
    std::vector<uint32_t> dense(bins, 0u);
    uint64_t final_hash = 0;

    const std::size_t len = dna.size();
    if (k <= 0 || static_cast<std::size_t>(k) > len) {
        for (int iter = 0; iter < iters; ++iter) {
            final_hash = checksum_counts(dense.data(), bins);
        }
        return final_hash;
    }

    // Pre-compute shift mask for rolling key update
    const uint32_t mask = (1u << (2 * k)) - 1u;

    for (int iter = 0; iter < iters; ++iter) {
        std::memset(dense.data(), 0, bins * sizeof(uint32_t));

        // Initial k-mer
        uint32_t key = encode_kmer(dna.data(), k);
        dense[key]++;

        // Rolling updates for remaining positions
        for (std::size_t i = 1; i + static_cast<std::size_t>(k) <= len; ++i) {
            char out_base = dna[i - 1];
            char in_base  = dna[i + static_cast<std::size_t>(k) - 1];

            uint32_t out_val = 0;
            switch (out_base) {
                case 'A': out_val = 0; break;
                case 'C': out_val = 1; break;
                case 'G': out_val = 2; break;
                case 'T': out_val = 3; break;
                default: out_val = 0; break;
            }

            uint32_t in_val = 0;
            switch (in_base) {
                case 'A': in_val = 0; break;
                case 'C': in_val = 1; break;
                case 'G': in_val = 2; break;
                case 'T': in_val = 3; break;
                default: in_val = 0; break;
            }

            key = ((key << 2) | in_val) & mask;
            dense[key]++;
        }

        final_hash = checksum_counts(dense.data(), bins);
    }
    return final_hash;
}