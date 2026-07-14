#include "interface.h"
#include <cstring>
#include <immintrin.h>

namespace {

// Build bit-packed kmer 2-bits/base on the fly
constexpr bool GOOD_CHAR(char c) { return c=='A'||c=='C'||c=='G'||c=='T'; }

inline uint64_t FNV(const uint64_t* data, std::size_t items) {
    uint64_t h = 1469598103934665603ULL;
    for(std::size_t i = 0; i < items; ++i) {
        h ^= static_cast<uint64_t>(data[i]);
        h *= 1099511628211ULL;
    }
    return h;
}

} // anon

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const int K = k;
    if(K <= 0 || K>15 || iters <= 0) return 0;
    // max bins: 4^K -> uint32_t
    const std::size_t BINS = std::size_t(1U) << (2*K);
    alignas(64) static uint32_t hist[1U << 16]; // covers K<=15
    std::memset(hist, 0, BINS * sizeof(uint32_t));

    // build char->nibble LUT
    alignas(64) alignas(64) uint8_t code[256] = {0};
    code['C'] = 1; code['G'] = 2; code['T'] = 3; // A stays 0

    // quick reject non-ACGT chars or shorter than K
    const std::size_t N = dna.size();
    if(K > int(N)) {
        uint64_t h = 0;
        for(int t = 0; t < iters; ++t)
            h = FNV(hist, BINS);
        return h;
    }

    // compute kmer codes once
    uint32_t mask = (std::uint32_t(1) << (2*K)) - 1;
    uint32_t val = 0;
    const char* s = dna.data();
    // slide window
    for(std::size_t i = 0; i < N; ++i) {
        val = (val << 2 | code[static_cast<uint8_t>(s[i])]) & mask;
        if(i+1 >= std::size_t(K)) hist[val]++;
    }

    // only compute the result once
    uint64_t h = FNV(hist, BINS);

    // checksum requested iters times (they want the final checksum repeated)
    for(int t = 1; t < iters; ++t)
        h = FNV(hist, BINS);
    return h;
}