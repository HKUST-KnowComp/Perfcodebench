#include "interface.h"
#include "xxhash.h"
#include <cstdint>
#include <cstring>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#define USE_X86_SIMD 1
#include <immintrin.h>
#else
#define USE_X86_SIMD 0
#endif

static inline const char* find_newline_scalar(const char* p, const char* end) {
    while (p < end && *p != '\n') ++p;
    return p;
}

#if USE_X86_SIMD

#if defined(__AVX2__)
static inline const char* find_newline_avx2(const char* p, const char* end) {
    const __m256i nl = _mm256_set1_epi8('\n');
    while (p + 32 <= end) {
        __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p));
        __m256i cmp = _mm256_cmpeq_epi8(chunk, nl);
        int mask = _mm256_movemask_epi8(cmp);
        if (mask != 0) {
            return p + __builtin_ctz(static_cast<unsigned>(mask));
        }
        p += 32;
    }
    return find_newline_scalar(p, end);
}
#define find_newline_fast find_newline_avx2
#elif defined(__SSE2__)
static inline const char* find_newline_sse2(const char* p, const char* end) {
    const __m128i nl = _mm_set1_epi8('\n');
    while (p + 16 <= end) {
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
        __m128i cmp = _mm_cmpeq_epi8(chunk, nl);
        int mask = _mm_movemask_epi8(cmp);
        if (mask != 0) {
            return p + __builtin_ctz(static_cast<unsigned>(mask));
        }
        p += 16;
    }
    return find_newline_scalar(p, end);
}
#define find_newline_fast find_newline_sse2
#else
#define find_newline_fast find_newline_scalar
#endif

#else
#define find_newline_fast find_newline_scalar
#endif

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* p = input.data();
    const char* const end = p + input.size();
    
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;
    
    while (p < end) {
        const char* line_end = find_newline_fast(p, end);
        acc ^= XXH64(p, static_cast<std::size_t>(line_end - p), 0);
        acc *= FNV_PRIME;
        p = (line_end < end) ? (line_end + 1) : line_end;
    }
    
    return acc;
}