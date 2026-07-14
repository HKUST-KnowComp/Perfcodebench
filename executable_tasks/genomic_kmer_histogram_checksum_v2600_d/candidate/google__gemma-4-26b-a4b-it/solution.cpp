#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {

// Fast lookup table for DNA bases to 2-bit representation
// A=0, C=1, G=2, T=3. Non-ACGT characters are handled by masking/resetting.
static const uint8_t DNA_LOOKUP[256] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0
};

// Manual initialization of lookup to ensure correctness
inline void init_lookup() {
    // This is a conceptual placeholder; in real code we'd use a static const array.
    // For the sake of this implementation, we'll use a local static.
}

// Using a more robust approach for the lookup to avoid initialization issues
struct DNA_Map {
    uint8_t map[256];
    DNA_Map() {
        for(int i=0; i<256; ++i) map[i] = 0;
        map['A'] = 0; map['C'] = 1; map['G'] = 2; map['T'] = 3;
    }
} const dna_map;

inline uint64_t checksum_counts(const std::vector<uint32_t>& counts) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t count : counts) {
        hash ^= static_cast<uint64_t>(count);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const std::size_t num_bins = 1u << (2 * k);
    const std::size_t n = dna.size();
    const std::size_t k_size = static_cast<std::size_t>(k);
    const uint32_t mask = (1u << (2 * k)) - 1;
    
    std::vector<uint32_t> dense(num_bins);
    uint64_t final_hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(dense.begin(), dense.end(), 0u);
        
        if (n >= k_size) {
            uint32_t current_kmer = 0;
            uint32_t valid_len = 0;

            for (std::size_t i = 0; i < n; ++i) {
                uint8_t val = 0;
                char c = dna[i];
                if (c == 'C') val = 1;
                else if (c == 'G') val = 2;
                else if (c == 'T') val = 3;
                else if (c == 'A') val = 0;
                else {
                    // Handle non-ACGT if necessary, though problem implies DNA
                    valid_len = 0;
                    current_kmer = 0;
                    continue;
                }

                current_kmer = ((current_kmer << 2) | val) & mask;
                valid_len++;

                if (valid_len >= k_size) {
                    dense[current_kmer]++;
                }
            }
        }
        final_hash = checksum_counts(dense);
    }

    return final_hash;
}
