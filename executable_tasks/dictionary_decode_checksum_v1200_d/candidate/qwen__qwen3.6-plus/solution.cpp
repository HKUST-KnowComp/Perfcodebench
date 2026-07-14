#include "interface.h"
#include <vector>

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
    
    const size_t n = codes.size();
    std::vector<uint64_t> precomputed(2 * n);
    
    for (size_t i = 0; i < n; ++i) {
        const std::string& s = dict[static_cast<size_t>(codes[i])];
        precomputed[2 * i] = s.size();
        precomputed[2 * i + 1] = s.empty() ? 0ULL : static_cast<uint64_t>(static_cast<unsigned char>(s[0]));
    }

    uint64_t hash = 0;
    const uint64_t FNV_PRIME = 1099511628211ULL;
    const uint64_t FNV_OFFSET = 1469598103934665603ULL;
    const uint64_t* data = precomputed.data();
    const size_t data_size = precomputed.size();

    for (int iter = 0; iter < iters; ++iter) {
        hash = FNV_OFFSET;
        for (size_t i = 0; i < data_size; ++i) {
            hash ^= data[i];
            hash *= FNV_PRIME;
        }
    }
    return hash;
}