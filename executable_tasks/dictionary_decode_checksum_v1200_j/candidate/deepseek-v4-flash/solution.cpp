#include "interface.h"
#include <string>
#include <vector>

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
    uint64_t hash = 0;  // Will be reset in loop, but needed for iters==0
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (auto code : codes) {
            const std::string& s = dict[static_cast<std::size_t>(code)];
            hash ^= s.size();
            hash *= 1099511628211ULL;
            hash ^= s.empty() ? 0 : static_cast<unsigned char>(s[0]);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}
