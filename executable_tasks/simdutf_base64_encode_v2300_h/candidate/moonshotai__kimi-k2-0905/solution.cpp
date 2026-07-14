#include "interface.h"

#include <immintrin.h>
#include <cstdint>
#include <cstring>

#if defined(__AVX2__)
#define SIMD_TARGET __attribute__((target("avx2")))
#else
#define SIMD_TARGET
#endif

namespace {

alignas(64) static const char kBase64Table[64] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

// 32-byte AVX2 FNV-1a hasher --------------------------------------------------
SIMD_TARGET inline uint64_t hash_avx2(const uint8_t* data, size_t len) {
    const __m256i prime = _mm256_set1_epi64x(0x100000001b3ULL);
    __m256i h = _mm256_set1_epi64x(0xcbf29ce484222325ULL);

    const uint8_t* end = data + (len & ~31ULL);
    for (; data < end; data += 32) {
        __m256i bytes = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data));
        h = _mm256_xor_si256(h, bytes);
        h = _mm256_mul_epu32(h, prime); // 32-bit mul OK, high garbage overwritten
    }

    uint64_t lanes[4];
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(lanes), h);
    uint64_t hash = lanes[0] ^ lanes[1] ^ lanes[2] ^ lanes[3];

    // scalar tail
    for (; data < end + (len & 31); ++data) {
        hash ^= *data;
        hash *= 0x100000001b3ULL;
    }
    return hash;
}

// SSSE3 16-byte base64 encoder ------------------------------------------------
SIMD_TARGET inline void encode16(const uint8_t* __restrict in,
                                 char* __restrict out) {
    const __m128i lut = _mm_loadu_si128(reinterpret_cast<const __m128i*>(kBase64Table));

    __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(in));

    // expand 12 bytes → 16 bytes via pshufb
    const __m128i shuffle = _mm_setr_epi8(
        0, 0, 0, 1, 0, 0, 1, 2, 0, 1, 2, 3, 1, 2, 3, 4);
    __m128i x = _mm_shuffle_epi8(chunk, shuffle);

    // shift/mask to get six-bit fields
    __m128i a = _mm_and_si128(_mm_srli_epi32(x, 2),  _mm_set1_epi32(0x3F));
    __m128i b = _mm_and_si128(_mm_srli_epi32(x, 12), _mm_set1_epi32(0x3F));
    __m128i c = _mm_and_si128(_mm_srli_epi32(x, 22), _mm_set1_epi32(0x3F));
    __m128i d = _mm_and_si128(_mm_srli_epi32(x, 32), _mm_set1_epi32(0x3F));

    __m128i indices = _mm_or_si128(_mm_or_si128(a, _mm_slli_si128(b, 4)),
                                   _mm_or_si128(_mm_slli_si128(c, 8),
                                                _mm_slli_si128(d, 12)));
    indices = _mm_shuffle_epi8(lut, indices);
    _mm_storeu_si128(reinterpret_cast<__m128i*>(out), indices);
}

// scalar fallback for tails < 16 bytes ----------------------------------------
inline void encode_tail(const uint8_t* in, size_t len, char* out) {
    static const char tbl[64] =
        "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
    for (size_t i = 0; i < len; i += 3) {
        uint32_t b0 = in[i];
        uint32_t b1 = (i + 1 < len) ? in[i + 1] : 0;
        uint32_t b2 = (i + 2 < len) ? in[i + 2] : 0;
        uint32_t triple = (b0 << 16) | (b1 << 8) | b2;
        out[0] = tbl[(triple >> 18) & 0x3F];
        out[1] = tbl[(triple >> 12) & 0x3F];
        out[2] = (i + 1 < len) ? tbl[(triple >> 6) & 0x3F] : '=';
        out[3] = (i + 2 < len) ? tbl[triple & 0x3F]          : '=';
        out += 4;
    }
}

} // namespace

uint64_t encode_checksum(const std::string& input) {
    const size_t len = input.size();
    const size_t full_blocks = len / 16;
    const size_t tail_len = len % 16;

    // worst-case: 4*(ceil(len/3)) bytes
    const size_t out_cap = ((len + 2) / 3) * 4;
    char* out = static_cast<char*>(malloc(out_cap));
    char* dst = out;

    const uint8_t* src = reinterpret_cast<const uint8_t*>(input.data());

    for (size_t b = 0; b < full_blocks; ++b) {
        encode16(src, dst);
        src += 16;
        dst += 24; // 16 → 24 bytes (16*4/3=21.33 rounded up)
    }

    // re-compute exact tail length & encode
    const size_t tail_start = full_blocks * 16;
    const size_t tail_bytes = len - tail_start;
    if (tail_bytes) {
        encode_tail(src, tail_bytes, dst);
        dst += ((tail_bytes + 2) / 3) * 4;
    }

    const size_t out_len = dst - out;
    uint64_t hash = hash_avx2(reinterpret_cast<const uint8_t*>(out), out_len);
    free(out);
    return hash;
}