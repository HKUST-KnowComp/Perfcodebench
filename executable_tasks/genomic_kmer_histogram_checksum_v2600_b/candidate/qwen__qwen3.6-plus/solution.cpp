#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <algorithm>

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const size_t bins = 1u << (2 * k);
    std::vector<uint32_t> counts(bins, 0);
    const uint32_t mask = (1u << (2 * k)) - 1;
    uint64_t hash = 0;
    const size_t n = dna.size();
    const char* s = dna.data();
    const size_t k_sz = static_cast<size_t>(k);

    for (int iter = 0; iter < iters; ++iter) {
        std::fill(counts.begin(), counts.end(), 0);
        uint32_t code = 0;
        for (size_t i = 0; i < n; ++i) {
            uint32_t v = (s[i] == 'C') ? 1 : (s[i] == 'G') ? 2 : (s[i] == 'T') ? 3 : 0;
            code = ((code << 2) & mask) | v;
            if (i + 1 >= k_sz) {
                counts[code]++;
            }
        }

        hash = 1469598103934665603ULL;
        for (uint32_t c : counts) {
            hash ^= static_cast<uint64_t>(c);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}
