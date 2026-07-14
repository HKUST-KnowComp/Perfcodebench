#include "interface.h"

#include <cstdint>
#include <cstring>

namespace {

alignas(64) constexpr char kTable[64] = {
    'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
};

__attribute__((always_inline)) inline uint64_t fnv_step(uint64_t h, unsigned char c) {
    return (h ^ c) * 1099511628211ULL;
}

}  // namespace

uint64_t encode_checksum(const std::string& input) {
    const std::size_t n = input.size();
    if (n == 0) {
        return 1469598103934665603ULL;
    }

    const unsigned char* src = reinterpret_cast<const unsigned char*>(input.data());
    uint64_t hash = 1469598103934665603ULL;

    const std::size_t full_triples = n / 3;
    const std::size_t remainder = n % 3;

    // Process full 3-byte groups
    for (std::size_t i = 0; i < full_triples; ++i) {
        const unsigned char* p = src + i * 3;
        const uint32_t b0 = p[0];
        const uint32_t b1 = p[1];
        const uint32_t b2 = p[2];
        const uint32_t triple = (b0 << 16) | (b1 << 8) | b2;

        const unsigned char c0 = static_cast<unsigned char>(kTable[(triple >> 18) & 0x3FU]);
        const unsigned char c1 = static_cast<unsigned char>(kTable[(triple >> 12) & 0x3FU]);
        const unsigned char c2 = static_cast<unsigned char>(kTable[(triple >> 6) & 0x3FU]);
        const unsigned char c3 = static_cast<unsigned char>(kTable[triple & 0x3FU]);

        hash = fnv_step(hash, c0);
        hash = fnv_step(hash, c1);
        hash = fnv_step(hash, c2);
        hash = fnv_step(hash, c3);
    }

    // Handle remainder
    if (remainder == 1) {
        const uint32_t b0 = src[full_triples * 3];
        const unsigned char c0 = static_cast<unsigned char>(kTable[(b0 >> 2) & 0x3FU]);
        const unsigned char c1 = static_cast<unsigned char>(kTable[(b0 << 4) & 0x3FU]);
        hash = fnv_step(hash, c0);
        hash = fnv_step(hash, c1);
        hash = fnv_step(hash, '=');
        hash = fnv_step(hash, '=');
    } else if (remainder == 2) {
        const uint32_t b0 = src[full_triples * 3];
        const uint32_t b1 = src[full_triples * 3 + 1];
        const uint32_t pair = (b0 << 8) | b1;
        const unsigned char c0 = static_cast<unsigned char>(kTable[(pair >> 10) & 0x3FU]);
        const unsigned char c1 = static_cast<unsigned char>(kTable[(pair >> 4) & 0x3FU]);
        const unsigned char c2 = static_cast<unsigned char>(kTable[(pair << 2) & 0x3FU]);
        hash = fnv_step(hash, c0);
        hash = fnv_step(hash, c1);
        hash = fnv_step(hash, c2);
        hash = fnv_step(hash, '=');
    }

    return hash;
}
