#include "interface.h"
#include "xxhash.h"
#include <cstring>

#if defined(__AVX512F__) && defined(__AVX512BW__)
#include <immintrin.h>
#define USE_AVX512 1
#elif defined(__AVX2__)
#include <immintrin.h>
#define USE_AVX2 1
#elif defined(__SSE2__)
#include <emmintrin.h>
#define USE_SSE2 1
#endif

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* p = input.data();
    const char* const end = p + input.size();
    
#if defined(USE_AVX512)
    const __m512i newline_vec = _mm512_set1_epi8('\n');
    
    while (p < end) {
        const char* line_end = p;
        
        // Use AVX-512 to scan for newlines in 64-byte chunks
        while (line_end + 64 <= end) {
            __m512i chunk = _mm512_loadu_si512(reinterpret_cast<const __m512i*>(line_end));
            __mmask64 mask = _mm512_cmpeq_epi8_mask(chunk, newline_vec);
            if (mask != 0) {
                line_end += __builtin_ctzll(mask);
                goto found_newline;
            }
            line_end += 64;
        }
        
        // Handle remaining bytes
        while (line_end < end && *line_end != '\n') {
            ++line_end;
        }
        
    found_newline:
        acc ^= XXH64(p, static_cast<size_t>(line_end - p), 0);
        acc *= 1099511628211ULL;
        p = (line_end < end) ? (line_end + 1) : line_end;
    }
    
#elif defined(USE_AVX2)
    const __m256i newline_vec = _mm256_set1_epi8('\n');
    
    while (p < end) {
        const char* line_end = p;
        
        // Use AVX2 to scan for newlines in 32-byte chunks
        while (line_end + 32 <= end) {
            __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(line_end));
            __m256i cmp = _mm256_cmpeq_epi8(chunk, newline_vec);
            int mask = _mm256_movemask_epi8(cmp);
            if (mask != 0) {
                line_end += __builtin_ctz(mask);
                goto found_newline_avx2;
            }
            line_end += 32;
        }
        
        // Handle remaining bytes
        while (line_end < end && *line_end != '\n') {
            ++line_end;
        }
        
    found_newline_avx2:
        acc ^= XXH64(p, static_cast<size_t>(line_end - p), 0);
        acc *= 1099511628211ULL;
        p = (line_end < end) ? (line_end + 1) : line_end;
    }
    
#elif defined(USE_SSE2)
    const __m128i newline_vec = _mm_set1_epi8('\n');
    
    while (p < end) {
        const char* line_end = p;
        
        // Use SSE2 to scan for newlines in 16-byte chunks
        while (line_end + 16 <= end) {
            __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(line_end));
            __m128i cmp = _mm_cmpeq_epi8(chunk, newline_vec);
            int mask = _mm_movemask_epi8(cmp);
            if (mask != 0) {
                line_end += __builtin_ctz(mask);
                goto found_newline_sse2;
            }
            line_end += 16;
        }
        
        // Handle remaining bytes
        while (line_end < end && *line_end != '\n') {
            ++line_end;
        }
        
    found_newline_sse2:
        acc ^= XXH64(p, static_cast<size_t>(line_end - p), 0);
        acc *= 1099511628211ULL;
        p = (line_end < end) ? (line_end + 1) : line_end;
    }
    
#else
    // Fallback using memchr which is typically well-optimized
    while (p < end) {
        const char* line_end = static_cast<const char*>(memchr(p, '\n', static_cast<size_t>(end - p)));
        if (line_end == nullptr) {
            line_end = end;
        }
        
        acc ^= XXH64(p, static_cast<size_t>(line_end - p), 0);
        acc *= 1099511628211ULL;
        p = (line_end < end) ? (line_end + 1) : line_end;
    }
#endif
    
    return acc;
}