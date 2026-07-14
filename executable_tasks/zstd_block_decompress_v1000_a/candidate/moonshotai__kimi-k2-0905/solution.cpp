#include "interface.h"

#include "zstd.h"

#include <immintrin.h>
#include <string>
#include <cstdint>

namespace {

uint64_t checksum_bytes(const std::string& data) {
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(data.data());
    size_t len = data.size();

    uint64_t hash = 1469598103934665603ULL;

#if defined(__AVX512F__)
    const __m512i prime = _mm512_set1_epi64(1099511628211ULL);
    __m512i acc = _mm512_set1_epi64(hash);
    size_t i = 0;
    for (; i + 64 <= len; i += 64) {
        __m512i chunk = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(ptr + i));
        acc = _mm512_xor_si512(acc, chunk);
        acc = _mm512_mullo_epi64(acc, prime);
    }
    uint64_t tmp[8];
    _mm512_storeu_si512(tmp, acc);
    for (int k = 0; k < 8; ++k) hash ^= tmp[k];
    for (; i < len; ++i) {
        hash ^= ptr[i];
        hash *= 1099511628211ULL;
    }
#elif defined(__AVX2__)
    const __m256i prime = _mm256_set1_epi64x(1099511628211ULL);
    __m256i acc = _mm256_set1_epi64x(hash);
    size_t i = 0;
    for (; i + 32 <= len; i += 32) {
        __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(ptr + i));
        acc = _mm256_xor_si256(acc, chunk);
        acc = _mm256_mullo_epi64(acc, prime);
    }
    uint64_t tmp[4];
    _mm256_storeu_si256(reinterpret_cast<__m256i*>(tmp), acc);
    for (int k = 0; k < 4; ++k) hash ^= tmp[k];
    for (; i < len; ++i) {
        hash ^= ptr[i];
        hash *= 1099511628211ULL;
    }
#elif defined(__SSE2__)
    const __m128i prime = _mm_set1_epi64x(1099511628211ULL);
    __m128i acc = _mm_set1_epi64x(hash);
    size_t i = 0;
    for (; i + 16 <= len; i += 16) {
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(ptr + i));
        acc = _mm_xor_si128(acc, chunk);
        acc = _mm_mullo_epi64(acc, prime);
    }
    uint64_t tmp[2];
    _mm_storeu_si128(reinterpret_cast<__m128i*>(tmp), acc);
    for (int k = 0; k < 2; ++k) hash ^= tmp[k];
    for (; i < len; ++i) {
        hash ^= ptr[i];
        hash *= 1099511628211ULL;
    }
#else
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(ptr[i]);
        hash *= 1099511628211ULL;
    }
#endif
    return hash;
}

} // namespace

uint64_t decompress_checksum(const std::string& compressed, std::size_t decompressed_size) {
    std::string restored;
    restored.resize(decompressed_size);
    const size_t n = ZSTD_decompress(
        restored.data(), restored.size(), compressed.data(), compressed.size());
    if (ZSTD_isError(n) || n != decompressed_size) {
        return 0;
    }
    return checksum_bytes(restored);
}