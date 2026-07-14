#include "interface.h"
#include <cstdint>
#include <immintrin.h>
#include <cstring>

namespace {
constexpr std::uint64_t kTable64 =
    0x4443424140393837ULL |     // ABCDEFG 0..6
    ((0x4746454443424141ULL) << 16); // adjust later

static alignas(64) const unsigned char kEncodeTab[64] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

static alignas(64) const unsigned char kPadTab[4] = "==";

// Branch-free 32-byte chunk encoder + rolling checksum
inline void encode32(const unsigned char* __restrict src,
                     unsigned char* __restrict dst,
                     std::size_t len,
                     std::uint64_t &h1, std::uint64_t &h1_mul) noexcept
{
    std::uint64_t w[12];           // 32*3 == 96 bits == 12*8bytes ; we hold the triples as u64le
    std::uint64_t in[8];           // 32 bytes loaded as 8 u64le
    std::memcpy(in, src, 32);
    const std::uint32_t maskA = 0x3F3F3F3F;
    const std::uint32_t maskB = 0x0FC0FC00;
    std::uint64_t v;
    // 0..5   0..5   0..5   0..5
    #define TRIPLE0(i) std::uint32_t((std::uint32_t(src[i])<<16) | (std::uint32_t(src[i+1])<<8) | std::uint32_t(src[i+2]))
    #define PACK6(x,shift) std::uint64_t((std::uint64_t((x)>>(shift)) & 0x3F))

    // --- process 10 triples (30 bytes) producing 40 output bytes ---
    for (unsigned i=0,t=0; i<10; i++,t+=4) {
        std::uint32_t triple = TRIPLE0(i*3);
        std::uint64_t c0 = kEncodeTab[PACK6(triple,18)];
        std::uint64_t c1 = kEncodeTab[PACK6(triple,12)];
        std::uint64_t c2 = kEncodeTab[PACK6(triple,6)];
        std::uint64_t c3 = kEncodeTab[PACK6(triple,0)];
        dst[t]   = static_cast<unsigned char>(c0);
        dst[t+1] = static_cast<unsigned char>(c1);
        dst[t+2] = static_cast<unsigned char>(c2);
        dst[t+3] = static_cast<unsigned char>(c3);
        // fold into running checksum
        h1 ^= c0; h1 *= 1099511628211ULL;
        h1 ^= c1; h1 *= 1099511628211ULL;
        h1 ^= c2; h1 *= 1099511628211ULL;
        h1 ^= c3; h1 *= 1099511628211ULL;
    }
    // --- last 2 bytes (3 in) -> 4 out ---
    {
        unsigned char b0=src[30], b1=src[31];
        std::uint32_t triple = (std::uint32_t(b0)<<16) | (std::uint32_t(b1)<<8);
        dst[40] = kEncodeTab[PACK6(triple,18)];
        dst[41] = kEncodeTab[PACK6(triple,12)];
        dst[42] = kEncodeTab[PACK6(triple,6)];
        dst[43] = '=';
        h1 ^= dst[40]; h1 *= 1099511628211ULL;
        h1 ^= dst[41]; h1 *= 1099511628211ULL;
        h1 ^= dst[42]; h1 *= 1099511628211ULL;
        h1 ^= '=';    h1 *= 1099511628211ULL;
    }
}
} // anonymous namespace

uint64_t encode_checksum(const std::string& input) {
    const std::size_t inLen  = input.size();
    const std::size_t outLen = ((inLen + 2)/3)*4;
    alignas(64) unsigned char buf[64];
    std::uint64_t hash = 1469598103934665603ULL;
    const unsigned char* __restrict src =
        reinterpret_cast<const unsigned char*>(input.data());
    unsigned char*      __restrict dst = buf;

    std::size_t i = 0;
    // bulk path: 32-byte chunks
    for (; i + 32 <= inLen; i += 32, dst += 44) {
        encode32(src + i, dst, 32, hash, hash);
    }
    // tail: 0..31 bytes remain
    std::size_t rem = inLen - i;
    if (rem == 0) {
        // already processed everything
        return hash;
    }
    // handle up to 31 remaining bytes in scalar branch-free fashion
    const unsigned char* p = src + i;
    for (; rem >= 3; rem -= 3, p += 3, dst += 4) {
        std::uint32_t triple = (std::uint32_t(p[0])<<16)|(std::uint32_t(p[1])<<8)|std::uint32_t(p[2]);
        dst[0] = kEncodeTab[(triple>>18)&0x3F];
        dst[1] = kEncodeTab[(triple>>12)&0x3F];
        dst[2] = kEncodeTab[(triple>>6)&0x3F];
        dst[3] = kEncodeTab[(triple>>0)&0x3F];
        // accumulate checksum
        hash ^= dst[0]; hash *= 1099511628211ULL;
        hash ^= dst[1]; hash *= 1099511628211ULL;
        hash ^= dst[2]; hash *= 1099511628211ULL;
        hash ^= dst[3]; hash *= 1099511628211ULL;
    }
    // 0,1,2 bytes left
    if (rem == 1) {
        std::uint32_t triple = std::uint32_t(p[0])<<16;
        dst[0] = kEncodeTab[(triple>>18)&0x3F];
        dst[1] = kEncodeTab[(triple>>12)&0x3F];
        dst[2] = '=';
        dst[3] = '=';
        hash ^= dst[0]; hash *= 1099511628211ULL;
        hash ^= dst[1]; hash *= 1099511628211ULL;
        hash ^= '=';    hash *= 1099511628211ULL;
        hash ^= '=';    hash *= 1099511628211ULL;
    } else if (rem == 2) {
        std::uint32_t triple = (std::uint32_t(p[0])<<16)|(std::uint32_t(p[1])<<8);
        dst[0] = kEncodeTab[(triple>>18)&0x3F];
        dst[1] = kEncodeTab[(triple>>12)&0x3F];
        dst[2] = kEncodeTab[(triple>>6)&0x3F];
        dst[3] = '=';
        hash ^= dst[0]; hash *= 1099511628211ULL;
        hash ^= dst[1]; hash *= 1099511628211ULL;
        hash ^= dst[2]; hash *= 1099511628211ULL;
        hash ^= '=';    hash *= 1099511628211ULL;
    }
    return hash;
}