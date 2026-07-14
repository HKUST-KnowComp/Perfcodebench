#include "interface.h"

#include "xxhash.h"
#include <cstring>

#if defined(__AVX2__)
#include <immintrin.h>
#endif

static inline const char* find_newline(const char* p, const char* end) {
#if defined(__AVX2__)
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
#endif
    const void* found = memchr(p, '\n', static_cast<size_t>(end - p));
    return found ? static_cast<const char*>(found) : end;
}

uint64_t hash_lines(const std::string& input) {
    uint64_t acc = 1469598103934665603ULL;
    const char* p = input.data();
    const char* const end = p + input.size();
    
    while (p < end) {
        const char* line_end = find_newline(p, end);
        size_t len = static_cast<size_t>(line_end - p);
        
        uint64_t h = XXH3_64bits(p, len);
        acc ^= h;
        acc *= 1099511628211ULL;
        
        p = (line_end < end) ? (line_end + 1) : end;
    }
    
    return acc;
}
