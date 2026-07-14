#include "interface.h"

#include "xxhash.h"
#include <cstring>

#if defined(__SSE2__)
#include <emmintrin.h>
#endif

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* p = input.data();
    const char* const end = p + input.size();
    
#if defined(__SSE2__)
    const __m128i newline_vec = _mm_set1_epi8('\n');
#endif
    
    while (p < end) {
        const char* line_end = p;
        std::size_t remaining = static_cast<std::size_t>(end - line_end);
        
#if defined(__SSE2__)
        // Use SIMD to scan for newlines in 16-byte chunks
        while (remaining >= 16) {
            __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(line_end));
            __m128i cmp = _mm_cmpeq_epi8(chunk, newline_vec);
            int mask = _mm_movemask_epi8(cmp);
            if (mask != 0) {
                // Found a newline in this chunk
                int offset = __builtin_ctz(mask);
                line_end += offset;
                goto found_newline;
            }
            line_end += 16;
            remaining -= 16;
        }
#endif
        
        // Scalar fallback for remaining bytes
        while (line_end < end && *line_end != '\n') {
            ++line_end;
        }
        
#if defined(__SSE2__)
    found_newline:
#endif
        acc ^= static_cast<uint64_t>(XXH64(p, static_cast<std::size_t>(line_end - p), 0));
        acc *= 1099511628211ULL;
        p = (line_end < end) ? (line_end + 1) : line_end;
    }
    return acc;
}
