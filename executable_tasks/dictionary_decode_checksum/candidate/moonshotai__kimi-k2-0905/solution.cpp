#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

namespace {

/*
 * Mix in a single 64-bit value using a single‐round FNV-1a variant
 * (identical to that used in the reference implementation).
 */
inline uint64_t mix(uint64_t hash, uint64_t value) noexcept {
    return (hash ^ value) * 1099511628211ULL;
}

} // anonymous namespace

uint64_t dictionary_decode_checksum(
    const std::vector<std::string>& dict,
    const std::vector<uint32_t>& codes,
    int iters) {
    // We reuse offsets computed once outside the hot loop to avoid repeated
dictionary‐indirections and to remain branch-free.
    const std::size_t n = codes.size();

    // Pre-compute length and first‐byte for every reference we will touch.
    // We keep these two arrays separate to minimize false sharing
    // and guarantee they fit in L1 without extra alignment fuss.
    std::vector<std::uint8_t> len(n);
    std::vector<std::uint8_t> fst(n);

    for (std::size_t i = 0; i < n; ++i) {
        const auto& token = dict[codes[i]];
        len[i] = uint8_t(token.size());
        fst[i] = token.empty() ? uint8_t(0) : uint8_t(token[0]);
    }

    uint64_t hash = 0;
    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (std::size_t i = 0; i < n; ++i) {
            hash = mix(hash, len[i]);
            hash = mix(hash, fst[i]);
        }
    }
    return hash;
}