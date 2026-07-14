#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

// Lookup table for DNA base to 2-bit code.
static const uint32_t base_code[256] = []() {
    uint32_t arr[256] = {0};
    arr['C'] = 1;
    arr['G'] = 2;
    arr['T'] = 3;
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

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int /*iters*/) {
    const size_t bins = 1ULL << (2 * k);
    std::vector<uint32_t> counts(bins, 0u);

    size_t n = dna.size();
    if (n < static_cast<size_t>(k)) {
        return checksum_counts(counts);
    }

    // Encode first k-mer.
    uint32_t code = 0;
    for (int i = 0; i < k; ++i) {
        code = (code << 2) | base_code[static_cast<unsigned char>(dna[i])];
    }
    counts[code]++;

    // Slide window over the rest of the string.
    uint32_t mask = (1U << (2 * k)) - 1;
    for (size_t i = k; i < n; ++i) {
        code = ((code << 2) | base_code[static_cast<unsigned char>(dna[i])]) & mask;
        counts[code]++;
    }

    return checksum_counts(counts);
}
