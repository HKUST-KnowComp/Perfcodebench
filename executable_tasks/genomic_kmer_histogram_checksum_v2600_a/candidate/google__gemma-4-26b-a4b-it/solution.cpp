#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

namespace {

// Precomputed lookup table for DNA base to 2-bit encoding
// A=0, C=1, G=2, T=3. Other chars map to 0 to avoid branching.
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

// Manual initialization of the lookup table for the specific characters
// This is done once via a helper if needed, but for simplicity we use a static array.
// In a real scenario, we'd use a constexpr or a static init block.
inline uint8_t get_dna_val(char c) {
    if (c == 'C') return 1;
    if (c == 'G') return 2;
    if (c == 'T') return 3;
    return 0;
}

// Since we can't easily use a static initializer in a single file without complexity,
// we'll use a simple switch or if-else inside the loop which is still fast.

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
            // Initialize first k-1 bases
            for (std::size_t i = 0; i < k_size - 1; ++i) {
                uint32_t val = 0;
                char c = dna[i];
                if (c == 'C') val = 1;
                else if (c == 'G') val = 2;
                else if (c == 'T') val = 3;
                current_kmer = (current_kmer << 2) | val;
            }

            // Slide through the rest
            for (std::size_t i = k_size - 1; i < n; ++i) {
                uint32_t val = 0;
                char c = dna[i];
                if (c == 'C') val = 1;
                else if (c == 'G') val = 2;
                else if (c == 'T') val = 3;
                
                current_kmer = ((current_kmer << 2) | val) & mask;
                dense[current_kmer]++;
            }
        }
        final_hash = checksum_counts(dense);
    }

    return final_hash;
}
