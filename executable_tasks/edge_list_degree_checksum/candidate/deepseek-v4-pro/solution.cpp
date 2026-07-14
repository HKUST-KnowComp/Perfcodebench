#include "interface.h"

#include <cstdint>
#include <vector>

namespace {
inline uint64_t mix(uint64_t hash, uint64_t value) {
    hash ^= value;
    hash *= 1099511628211ULL;
    return hash;
}
} // namespace

uint64_t degree_checksum(
    const std::vector<uint32_t>& src,
    const std::vector<uint32_t>& dst,
    uint32_t vertices,
    int iters) {
    (void)dst;
    if (iters <= 0) {
        return 0;
    }
    std::vector<uint32_t> degrees(vertices, 0);
    for (uint32_t u : src) {
        degrees[u]++;
    }
    uint64_t hash = 1469598103934665603ULL;
    for (uint32_t d : degrees) {
        hash = mix(hash, d);
    }
    return hash;
}
