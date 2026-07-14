#include "interface.h"
#include <cstdint>
#include <string>
#include <cstring>

namespace {

inline uint32_t encode_base(char c) noexcept {
    return (c >> 1) & 3;   // A(0x41)->0, C(0x43)->1, G(0x47)->2, T(0x54)->3
}

// 64-bit FNV-1a checksum
uint64_t checksum_counts(const uint32_t* counts, std::size_t bins) {
    uint64_t h = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    for (std::size_t i = 0; i < bins; ++i) {
        h ^= counts[i];
        h *= prime;
    }
    return h;
}

} // namespace

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    const std::size_t bins = 1ULL << (k * 2);
    if (bins == 0) return 0; // defensive

    alignas(64) static thread_local uint32_t dense[1 << 16]; // 2^16 = 65536 max for k=8

    const std::size_t len = dna.size();
    const int k1 = k - 1;

    uint64_t final_hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        std::memset(dense, 0, bins * sizeof(uint32_t));

        if (len < static_cast<std::size_t>(k)) {
            final_hash = checksum_counts(dense, bins);
            continue;
        }

        // Prime the first k-mer
        uint32_t key = 0;
        const char* s = dna.data();
        for (int i = 0; i < k; ++i) {
            key = (key << 2) | encode_base(s[i]);
        }
        ++dense[key];

        const std::size_t last = len - k;
        for (std::size_t i = 1; i <= last; ++i) {
            key = ((key << 2) | encode_base(s[i + k1])) & (bins - 1);
            ++dense[key];
        }

        final_hash = checksum_counts(dense, bins);
    }
    return final_hash;
}