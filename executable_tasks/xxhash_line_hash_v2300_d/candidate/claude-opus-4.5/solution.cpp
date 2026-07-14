#include "interface.h"
#include "xxhash.h"
#include <cstdint>
#include <cstring>

#if defined(__AVX2__)
#include <immintrin.h>
#define USE_AVX2 1
#elif defined(__SSE2__)
#include <emmintrin.h>
#define USE_SSE2 1
#endif

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* data = input.data();
    const size_t len = input.size();
    size_t start = 0;

#if defined(USE_AVX2)
    const __m256i newline_vec = _mm256_set1_epi8('\n');
    while (start < len) {
        size_t pos = start;
        // SIMD scan for newline
        while (pos + 32 <= len) {
            __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(data + pos));
            __m256i cmp = _mm256_cmpeq_epi8(chunk, newline_vec);
            int mask = _mm256_movemask_epi8(cmp);
            if (mask != 0) {
                size_t offset = __builtin_ctz(mask);
                size_t end = pos + offset;
                acc ^= XXH64(data + start, end - start, 0);
                acc *= 1099511628211ULL;
                start = end + 1;
                goto next_line_avx2;
            }
            pos += 32;
        }
        // Scalar fallback for remainder
        while (pos < len && data[pos] != '\n') {
            ++pos;
        }
        acc ^= XXH64(data + start, pos - start, 0);
        acc *= 1099511628211ULL;
        start = pos + 1;
        next_line_avx2:;
    }
#elif defined(USE_SSE2)
    const __m128i newline_vec = _mm_set1_epi8('\n');
    while (start < len) {
        size_t pos = start;
        // SIMD scan for newline
        while (pos + 16 <= len) {
            __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(data + pos));
            __m128i cmp = _mm_cmpeq_epi8(chunk, newline_vec);
            int mask = _mm_movemask_epi8(cmp);
            if (mask != 0) {
                size_t offset = __builtin_ctz(mask);
                size_t end = pos + offset;
                acc ^= XXH64(data + start, end - start, 0);
                acc *= 1099511628211ULL;
                start = end + 1;
                goto next_line_sse2;
            }
            pos += 16;
        }
        // Scalar fallback for remainder
        while (pos < len && data[pos] != '\n') {
            ++pos;
        }
        acc ^= XXH64(data + start, pos - start, 0);
        acc *= 1099511628211ULL;
        start = pos + 1;
        next_line_sse2:;
    }
#else
    // Pure scalar version
    while (start < len) {
        const char* line_start = data + start;
        const char* found = static_cast<const char*>(memchr(line_start, '\n', len - start));
        size_t line_len;
        if (found) {
            line_len = found - line_start;
            start = (found - data) + 1;
        } else {
            line_len = len - start;
            start = len + 1;
        }
        acc ^= XXH64(line_start, line_len, 0);
        acc *= 1099511628211ULL;
    }
#endif

    return acc;
}