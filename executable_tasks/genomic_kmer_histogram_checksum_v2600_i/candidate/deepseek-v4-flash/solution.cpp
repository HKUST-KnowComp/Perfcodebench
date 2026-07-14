#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const std::size_t bins = 1ULL << (2 * k);
    const uint32_t mask = static_cast<uint32_t>(bins - 1);
    std::vector<uint32_t> dense(bins, 0);

    // Character to 2-bit code: A=0, C=1, G=2, T=3
    unsigned char char_to_code[256] = {0};
    char_to_code['A'] = 0;
    char_to_code['C'] = 1;
    char_to_code['G'] = 2;
    char_to_code['T'] = 3;

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        std::fill(dense.begin(), dense.end(), 0);
        const std::size_t len = dna.size();
        if (len >= static_cast<std::size_t>(k)) {
            // Encode first k-mer
            uint32_t code = 0;
            for (int i = 0; i < k; ++i) {
                code = (code << 2) | char_to_code[static_cast<unsigned char>(dna[i])];
            }
            dense[code]++;
            // Slide
            for (std::size_t i = static_cast<std::size_t>(k); i < len; ++i) {
                code = ((code << 2) | char_to_code[static_cast<unsigned char>(dna[i])]) & mask;
                dense[code]++;
            }
        }
        // Compute FNV-1a checksum over dense counts
        hash = 1469598103934665603ULL;
        for (uint32_t count : dense) {
            hash ^= static_cast<uint64_t>(count);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}
