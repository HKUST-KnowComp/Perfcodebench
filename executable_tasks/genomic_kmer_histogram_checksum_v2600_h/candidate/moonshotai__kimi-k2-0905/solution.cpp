#include "interface.h"

#include <cstdint>
#include <cstring>
#include <string>
#include <vector>

namespace {

inline uint32_t encode_base(char c) noexcept {
    return (c & 6) >> 1;          // A=0, C=1, G=2, T=3
}

uint64_t checksum_counts(const uint32_t* __restrict dense, std::size_t bins) noexcept {
    const uint64_t prime = 1099511628211ULL;
    uint64_t h0 = 1469598103934665603ULL;
    uint64_t h1 = h0, h2 = h0, h3 = h0, h4 = h0, h5 = h0, h6 = h0, h7 = h0;
    std::size_t i = 0;
    for (; i + 8 <= bins; i += 8) {
        h0 ^= dense[i+0]; h0 *= prime;
        h1 ^= dense[i+1]; h1 *= prime;
        h2 ^= dense[i+2]; h2 *= prime;
        h3 ^= dense[i+3]; h3 *= prime;
        h4 ^= dense[i+4]; h4 *= prime;
        h5 ^= dense[i+5]; h5 *= prime;
        h6 ^= dense[i+6]; h6 *= prime;
        h7 ^= dense[i+7]; h7 *= prime;
    }
    h0 ^= h1; h0 *= prime;
    h0 ^= h2; h0 *= prime;
    h0 ^= h3; h0 *= prime;
    h0 ^= h4; h0 *= prime;
    h0 ^= h5; h0 *= prime;
    h0 ^= h6; h0 *= prime;
    h0 ^= h7; h0 *= prime;
    for (; i < bins; ++i) {
        h0 ^= dense[i];
        h0 *= prime;
    }
    return h0;
}

}  // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const std::size_t ksz = static_cast<std::size_t>(k);
    const std::size_t bins = 1u << (2 * k);
    std::vector<uint32_t> dense(bins, 0u);
    const std::size_t len = dna.size();
    const char* __restrict s = dna.data();
    uint64_t final_hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        std::memset(dense.data(), 0, bins * sizeof(uint32_t));
        if (__builtin_expect(len >= ksz, 1)) {
            uint32_t code = 0;
            for (std::size_t i = 0; i < ksz - 1; ++i) {
                code = (code << 2) | encode_base(s[i]);
            }
            const std::size_t last = len - ksz + 1;
            for (std::size_t i = 0; i < last; ++i) {
                code = ((code << 2) | encode_base(s[i + ksz - 1])) & ((1u << (2 * k)) - 1);
                dense[code]++;
            }
        }
        final_hash = checksum_counts(dense.data(), bins);
    }
    return final_hash;
}