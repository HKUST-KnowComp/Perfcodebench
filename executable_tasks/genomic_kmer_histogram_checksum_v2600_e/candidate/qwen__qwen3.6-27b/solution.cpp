#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <cstring>

namespace {

static const std::array<uint32_t, 256> base_to_val = []() {
    std::array<uint32_t, 256> arr{};
    arr['A'] = 0; arr['C'] = 1; arr['G'] = 2; arr['T'] = 3;
    arr['a'] = 0; arr['c'] = 1; arr['g'] = 2; arr['t'] = 3;
    return arr;
}();

uint64_t checksum_counts(const std::vector<uint32_t>& counts) {
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t count : counts) {
        hash ^= static_cast<uint64_t>(count);
        hash *= 1099511628211ULL;
    }
    return hash;
}

}  // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const size_t n = dna.size();
    const size_t bins = 1u << (2 * k);
    
    if (n < static_cast<size_t>(k)) {
        std::vector<uint32_t> dense(bins, 0u);
        return checksum_counts(dense);
    }

    std::vector<uint32_t> dense(bins, 0u);
    uint32_t mask = (1u << (2 * (k - 1))) - 1;
    
    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        std::memset(dense.data(), 0, bins * sizeof(uint32_t));
        
        uint32_t code = 0;
        for (int i = 0; i < k; ++i) {
            code = (code << 2) | base_to_val[static_cast<unsigned char>(dna[i])];
        }
        dense[code]++;
        
        for (size_t i = k; i < n; ++i) {
            code = ((code & mask) << 2) | base_to_val[static_cast<unsigned char>(dna[i])];
            dense[code]++;
        }
        
        hash = checksum_counts(dense);
    }
    return hash;
}