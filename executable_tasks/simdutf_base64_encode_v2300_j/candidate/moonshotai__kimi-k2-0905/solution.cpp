#include "interface.h"

#include <immintrin.h>
#include <string>

namespace {

/* 64-byte aligned base64 table; the low half is filled with the 64 characters,
   the high half is used for clamping. */
alignas(64) static const char kB64[128] = {
    'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/',
    /* high half mirrors low for clamp-free gather */
    'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
    'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
    'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
    'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
};

inline uint64_t update_hash(uint64_t hash, const void* ptr, std::size_t len) {
    const uint8_t* p = static_cast<const uint8_t*>(ptr);
    for (std::size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(p[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

/* process 32 input bytes → 44 bytes of output; the last 4 bytes of a block
   are handled at block boundaries, keeping tail handling simple. */
inline uint64_t encode_avx2(const char* in, char* out, uint64_t hash, std::size_t& inDone) {
    const __m256i pack = _mm256_set_epi8(
        10, 11, 12, 13, 14, 15, 4, 5, 6, 7, 8, 9, 12, 13, 14, 15,
         2,  3,  4,  5,  6,  7, 8, 9, 0, 1, 2, 3, 4, 5, 6, 7);
    const __m256i lo_nib = _mm256_set1_epi8(0x0F);
    const __m256i hi_mask = _mm256_set1_epi8(0x30);

    __m256i chunk0 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(in));
    __m256i chunk1 = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(in + 16));

    __m256i i0 = _mm256_shuffle_epi8(chunk0, pack);
    __m256i bitsLow  = _mm256_and_si256(i0, lo_nib);
    __m256i bitsHigh = _mm256_and_si256(_mm256_srli_epi32(i0, 4), hi_mask);
    __m256i idx = _mm256_or_si256(bitsLow, bitsHigh);

    __m128i low16 = _mm256_castsi256_si128(idx);
    __m128i high16 = _mm256_extracti128_si256(idx, 1);

    __m128i r0 = _mm_setzero_si128();
    r0 = _mm_insert_epi64(r0, *reinterpret_cast<const int64_t*>(&kB64[0]), 0);
    r0 = _mm_insert_epi64(r0, *reinterpret_cast<const int64_t*>(&kB64[8]), 1);

    __m128i b0 = _mm_shuffle_epi8(r0, low16);
    _mm_storeu_si128(reinterpret_cast<__m128i*>(out), b0);
    hash = update_hash(hash, out, 16);

    __m128i r1 = _mm_setzero_si128();
    r1 = _mm_insert_epi64(r1, *reinterpret_cast<const int64_t*>(&kB64[0]), 0);
    r1 = _mm_insert_epi64(r1, *reinterpret_cast<const int64_t*>(&kB64[8]), 1);

    __m128i b1 = _mm_shuffle_epi8(r1, high16);
    _mm_storeu_si128(reinterpret_cast<__m128i*>(out + 16), b1);
    hash = update_hash(hash, out + 16, 16);

    /* last 12 bytes from the second chunk yield 12 output bytes */
    __m256i i1 = _mm256_shuffle_epi8(chunk1, _mm256_set_epi8(
        6,7,8,9,10,11,12,13,14,15,0,1,2,3,4,5, 2,3,4,5,6,7,8,9,10,11,12,13,14,15,0,1));

    bitsLow  = _mm256_and_si256(i1, lo_nib);
    bitsHigh = _mm256_and_si256(_mm256_srli_epi32(i1, 4), hi_mask);
    idx = _mm256_or_si256(bitsLow, bitsHigh);

    low16 = _mm256_castsi256_si128(idx);
    high16 = _mm256_extracti128_si256(idx, 1);

    __m128i b2 = _mm_shuffle_epi8(r0, low16);
    __m128i b3 = _mm_shuffle_epi8(r1, high16);

    _mm_storeu_si128(reinterpret_cast<__m128i*>(out + 32), b2);
    hash = update_hash(hash, out + 32, 16);
    _mm_storeu_si128(reinterpret_cast<__m128i*>(out + 48), b3);
    hash = update_hash(hash, out + 48, 4);   /* only 4 valid for this register part */

    inDone = 32;
    return hash;
}

} // anonymous

uint64_t encode_checksum(const std::string& input) {
    const char* src = input.data();
    const std::size_t len = input.size();
    std::string out;
    out.reserve(((len + 2) / 3) * 4);
    char* dst = out.data();

    uint64_t hash = 1469598103934665603ULL;

    /* SIMD loop handles 32 input bytes per iteration */
    std::size_t off = 0;
    for (; off + 32 <= len; off += 32) {
        std::size_t consumed = 0;
        hash = encode_avx2(src + off, dst, hash, consumed);
        dst += 44; /* produced 44 bytes */
    }

    /* tail: scalar, no padding yet */
    for (; off + 3 <= len; off += 3) {
        const uint32_t b0 = static_cast<uint8_t>(src[off]);
        const uint32_t b1 = static_cast<uint8_t>(src[off + 1]);
        const uint32_t b2 = static_cast<uint8_t>(src[off + 2]);
        const uint32_t triple = (b0 << 16) | (b1 << 8) | b2;
        *dst++ = kB64[(triple >> 18) & 0x3F];
        *dst++ = kB64[(triple >> 12) & 0x3F];
        *dst++ = kB64[(triple >> 6) & 0x3F];
        *dst++ = kB64[triple & 0x3F];
    }

    /* last 0-2 bytes followed by padding */
    if (off < len) {
        const uint32_t b0 = static_cast<uint8_t>(src[off]);
        const uint32_t b1 = (off + 1 < len) ? static_cast<uint8_t>(src[off + 1]) : 0;
        const uint32_t b2 = 0;
        const uint32_t triple = (b0 << 16) | (b1 << 8) | b2;
        *dst++ = kB64[(triple >> 18) & 0x3F];
        *dst++ = kB64[(triple >> 12) & 0x3F];
        *dst++ = (off + 1 < len) ? kB64[(triple >> 6) & 0x3F] : '=';
        *dst++ = '=';
    }

    hash = update_hash(hash, out.data(), dst - out.data());
    return hash;
}
