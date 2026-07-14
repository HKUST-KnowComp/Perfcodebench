#include "interface.h"
#include <cstdint>
#include <cstring>

#if defined(__AVX2__)
#include <immintrin.h>
#define USE_AVX2 1
#elif defined(__SSE4_2__) || defined(__SSE2__)
#include <emmintrin.h>
#include <tmmintrin.h>
#define USE_SSE 1
#endif

#ifdef _OPENMP
#include <omp.h>
#endif

namespace {

inline bool valid_scalar(const unsigned char* p, const unsigned char* end) {
    while (p < end) {
        const unsigned char c = *p++;
        if (c <= 0x7F) continue;
        if (c < 0xC2) return false;
        if (c <= 0xDF) {
            if (p >= end || (*p & 0xC0U) != 0x80U) return false;
            ++p;
            continue;
        }
        if (c <= 0xEF) {
            if (end - p < 2) return false;
            unsigned char c1 = p[0], c2 = p[1];
            if ((c1 & 0xC0U) != 0x80U || (c2 & 0xC0U) != 0x80U) return false;
            if (c == 0xE0 && c1 < 0xA0) return false;
            if (c == 0xED && c1 > 0x9F) return false;
            p += 2;
            continue;
        }
        if (c <= 0xF4) {
            if (end - p < 3) return false;
            unsigned char c1 = p[0], c2 = p[1], c3 = p[2];
            if ((c1 & 0xC0U) != 0x80U || (c2 & 0xC0U) != 0x80U || (c3 & 0xC0U) != 0x80U) return false;
            if (c == 0xF0 && c1 < 0x90) return false;
            if (c == 0xF4 && c1 > 0x8F) return false;
            p += 3;
            continue;
        }
        return false;
    }
    return true;
}

#if USE_AVX2

inline bool validate_utf8_avx2(const char* data, std::size_t len) {
    const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
    const unsigned char* end = p + len;
    
    // Process 32 bytes at a time for ASCII fast path
    while (end - p >= 32) {
        __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p));
        // Check if all bytes are ASCII (high bit clear)
        int mask = _mm256_movemask_epi8(chunk);
        if (mask == 0) {
            p += 32;
            continue;
        }
        // Found non-ASCII, validate this chunk with scalar
        // Find first non-ASCII byte
        int first_non_ascii = __builtin_ctz(static_cast<unsigned>(mask));
        p += first_non_ascii;
        
        // Validate from here with scalar until we're back to aligned ASCII
        const unsigned char* chunk_end = p + (32 - first_non_ascii);
        if (chunk_end > end) chunk_end = end;
        
        while (p < chunk_end) {
            unsigned char c = *p;
            if (c <= 0x7F) { ++p; continue; }
            if (c < 0xC2) return false;
            if (c <= 0xDF) {
                if (p + 1 >= end || (p[1] & 0xC0U) != 0x80U) return false;
                p += 2;
            } else if (c <= 0xEF) {
                if (p + 2 >= end) return false;
                unsigned char c1 = p[1], c2 = p[2];
                if ((c1 & 0xC0U) != 0x80U || (c2 & 0xC0U) != 0x80U) return false;
                if (c == 0xE0 && c1 < 0xA0) return false;
                if (c == 0xED && c1 > 0x9F) return false;
                p += 3;
            } else if (c <= 0xF4) {
                if (p + 3 >= end) return false;
                unsigned char c1 = p[1], c2 = p[2], c3 = p[3];
                if ((c1 & 0xC0U) != 0x80U || (c2 & 0xC0U) != 0x80U || (c3 & 0xC0U) != 0x80U) return false;
                if (c == 0xF0 && c1 < 0x90) return false;
                if (c == 0xF4 && c1 > 0x8F) return false;
                p += 4;
            } else {
                return false;
            }
        }
    }
    
    return valid_scalar(p, end);
}

#elif USE_SSE

inline bool validate_utf8_sse(const char* data, std::size_t len) {
    const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
    const unsigned char* end = p + len;
    
    while (end - p >= 16) {
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
        int mask = _mm_movemask_epi8(chunk);
        if (mask == 0) {
            p += 16;
            continue;
        }
        int first_non_ascii = __builtin_ctz(static_cast<unsigned>(mask));
        p += first_non_ascii;
        
        const unsigned char* chunk_end = p + (16 - first_non_ascii);
        if (chunk_end > end) chunk_end = end;
        
        while (p < chunk_end) {
            unsigned char c = *p;
            if (c <= 0x7F) { ++p; continue; }
            if (c < 0xC2) return false;
            if (c <= 0xDF) {
                if (p + 1 >= end || (p[1] & 0xC0U) != 0x80U) return false;
                p += 2;
            } else if (c <= 0xEF) {
                if (p + 2 >= end) return false;
                unsigned char c1 = p[1], c2 = p[2];
                if ((c1 & 0xC0U) != 0x80U || (c2 & 0xC0U) != 0x80U) return false;
                if (c == 0xE0 && c1 < 0xA0) return false;
                if (c == 0xED && c1 > 0x9F) return false;
                p += 3;
            } else if (c <= 0xF4) {
                if (p + 3 >= end) return false;
                unsigned char c1 = p[1], c2 = p[2], c3 = p[3];
                if ((c1 & 0xC0U) != 0x80U || (c2 & 0xC0U) != 0x80U || (c3 & 0xC0U) != 0x80U) return false;
                if (c == 0xF0 && c1 < 0x90) return false;
                if (c == 0xF4 && c1 > 0x8F) return false;
                p += 4;
            } else {
                return false;
            }
        }
    }
    
    return valid_scalar(p, end);
}

#endif

inline bool validate_string(const std::string& s) {
    if (s.empty()) return true;
    
#if USE_AVX2
    return validate_utf8_avx2(s.data(), s.size());
#elif USE_SSE
    return validate_utf8_sse(s.data(), s.size());
#else
    return valid_scalar(reinterpret_cast<const unsigned char*>(s.data()),
                        reinterpret_cast<const unsigned char*>(s.data() + s.size()));
#endif
}

} // namespace

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
    std::size_t count = 0;
    const std::size_t n = inputs.size();
    
#ifdef _OPENMP
    if (n > 100) {
        #pragma omp parallel for reduction(+:count) schedule(dynamic, 64)
        for (std::size_t i = 0; i < n; ++i) {
            count += validate_string(inputs[i]) ? 1U : 0U;
        }
        return count;
    }
#endif
    
    for (std::size_t i = 0; i < n; ++i) {
        count += validate_string(inputs[i]) ? 1U : 0U;
    }
    return count;
}