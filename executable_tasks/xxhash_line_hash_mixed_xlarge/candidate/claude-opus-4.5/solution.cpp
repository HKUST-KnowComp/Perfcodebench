#include "interface.h"
#include "xxhash.h"
#include <cstdint>
#include <cstring>

#if defined(__AVX2__)
#include <immintrin.h>
#endif

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* p = input.data();
    const char* const end = p + input.size();
    
#if defined(__AVX2__)
    const __m256i newline_vec = _mm256_set1_epi8('\n');
    
    while (p < end) {
        const char* line_end = p;
        
        // AVX2 scan: 32 bytes at a time
        while (line_end + 32 <= end) {
            __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(line_end));
            __m256i cmp = _mm256_cmpeq_epi8(chunk, newline_vec);
            int mask = _mm256_movemask_epi8(cmp);
            if (mask != 0) {
                line_end += __builtin_ctz(mask);
                goto found_newline;
            }
            line_end += 32;
        }
        
        // Scalar fallback for remainder
        while (line_end < end && *line_end != '\n') {
            ++line_end;
        }
        
    found_newline:
        acc ^= XXH64(p, static_cast<size_t>(line_end - p), 0);
        acc *= 1099511628211ULL;
        p = (line_end < end) ? (line_end + 1) : line_end;
    }
#else
    // Non-AVX2 path: use word-at-a-time scanning
    while (p < end) {
        const char* line_end = p;
        
        // Word-at-a-time scan for newlines
        while (line_end + 8 <= end) {
            uint64_t word;
            memcpy(&word, line_end, 8);
            // Check if any byte is newline (0x0a)
            // XOR with 0x0a0a0a0a0a0a0a0a to make newlines into zeros
            uint64_t xored = word ^ 0x0a0a0a0a0a0a0a0aULL;
            // Use the standard zero-byte detection trick
            uint64_t has_zero = (xored - 0x0101010101010101ULL) & ~xored & 0x8080808080808080ULL;
            if (has_zero) {
                // Found a newline in this word, scan byte by byte
                while (line_end < end && *line_end != '\n') {
                    ++line_end;
                }
                goto found_nl;
            }
            line_end += 8;
        }
        
        // Scalar fallback
        while (line_end < end && *line_end != '\n') {
            ++line_end;
        }
        
    found_nl:
        acc ^= XXH64(p, static_cast<size_t>(line_end - p), 0);
        acc *= 1099511628211ULL;
        p = (line_end < end) ? (line_end + 1) : line_end;
    }
#endif
    
    return acc;
}
