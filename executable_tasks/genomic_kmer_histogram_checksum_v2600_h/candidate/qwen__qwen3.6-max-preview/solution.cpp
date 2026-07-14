#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>
#include <cstring>

uint64_t kmer_histogram_checksum(const std::string& dna, int k, int iters) {
    if (k <= 0 || static_cast<size_t>(k) > dna.size()) {
        const size_t bins = (k <= 0) ? 1 : (1ULL << (2 * k));
        std::vector<uint32_t> dense(bins, 0);
        uint64_t hash = 1469598103934665603ULL;
        for (uint32_t count : dense) {
            hash ^= count;
            hash *= 1099511628211ULL;
        }
        return hash;
    }

    const size_t bins = 1ULL << (2 * k);
    std::vector<uint32_t> dense(bins, 0);

    uint8_t enc[256];
    std::memset(enc, 0, sizeof(enc));
    enc[static_cast<unsigned char>('C')] = 1;
    enc[static_cast<unsigned char>('G')] = 2;
    enc[static_cast<unsigned char>('T')] = 3;

    uint32_t mask = static_cast<uint32_t>((1ULL << (2 * k)) - 1);
    uint32_t current = 0;
    int len = 0;

    for (unsigned char c : dna) {
        current = ((current << 2) & mask) | enc[c];
        if (++len >= k) {
            dense[current]++;
        }
    }

    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t count : dense) {
        hash ^= count;
        hash *= 1099511628211ULL;
    }

    (void)iters;
    return hash;
}
