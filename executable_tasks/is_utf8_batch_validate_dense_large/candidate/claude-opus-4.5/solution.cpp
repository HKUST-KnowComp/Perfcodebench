#include "interface.h"
#include <cstdint>
#include <cstddef>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <immintrin.h>
#define HAS_SSE2 1
#if defined(__AVX2__)
#define HAS_AVX2 1
#endif
#endif

namespace {

inline bool validate_utf8_fast(const unsigned char* p, std::size_t len) {
    const unsigned char* end = p + len;
    
#ifdef HAS_AVX2
    // AVX2 path: process 32 bytes at a time for ASCII
    while (p + 32 <= end) {
        __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p));
        __m256i high_bits = _mm256_and_si256(chunk, _mm256_set1_epi8(static_cast<char>(0x80)));
        if (_mm256_testz_si256(high_bits, high_bits)) {
            p += 32;
            continue;
        }
        // Has non-ASCII, process byte by byte until we're past non-ASCII
        break;
    }
#elif defined(HAS_SSE2)
    // SSE2 path: process 16 bytes at a time for ASCII
    while (p + 16 <= end) {
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
        int mask = _mm_movemask_epi8(chunk);
        if (mask == 0) {
            p += 16;
            continue;
        }
        break;
    }
#endif

    // Scalar validation for remaining bytes
    while (p < end) {
        unsigned char c = *p;
        
        if (c <= 0x7F) {
            // ASCII - try to batch process
#ifdef HAS_AVX2
            if (p + 32 <= end) {
                __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p));
                __m256i high_bits = _mm256_and_si256(chunk, _mm256_set1_epi8(static_cast<char>(0x80)));
                if (_mm256_testz_si256(high_bits, high_bits)) {
                    p += 32;
                    continue;
                }
            }
#elif defined(HAS_SSE2)
            if (p + 16 <= end) {
                __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
                int mask = _mm_movemask_epi8(chunk);
                if (mask == 0) {
                    p += 16;
                    continue;
                }
            }
#endif
            ++p;
            continue;
        }
        
        // 2-byte sequence: 110xxxxx 10xxxxxx
        if ((c & 0xE0) == 0xC0) {
            if (c < 0xC2) return false; // overlong
            if (p + 1 >= end) return false;
            if ((p[1] & 0xC0) != 0x80) return false;
            p += 2;
            continue;
        }
        
        // 3-byte sequence: 1110xxxx 10xxxxxx 10xxxxxx
        if ((c & 0xF0) == 0xE0) {
            if (p + 2 >= end) return false;
            unsigned char c1 = p[1];
            unsigned char c2 = p[2];
            if ((c1 & 0xC0) != 0x80 || (c2 & 0xC0) != 0x80) return false;
            // Check for overlong and surrogate
            if (c == 0xE0 && c1 < 0xA0) return false; // overlong
            if (c == 0xED && c1 > 0x9F) return false; // surrogate
            p += 3;
            continue;
        }
        
        // 4-byte sequence: 11110xxx 10xxxxxx 10xxxxxx 10xxxxxx
        if ((c & 0xF8) == 0xF0) {
            if (c > 0xF4) return false; // beyond Unicode
            if (p + 3 >= end) return false;
            unsigned char c1 = p[1];
            unsigned char c2 = p[2];
            unsigned char c3 = p[3];
            if ((c1 & 0xC0) != 0x80 || (c2 & 0xC0) != 0x80 || (c3 & 0xC0) != 0x80) return false;
            // Check for overlong and beyond Unicode
            if (c == 0xF0 && c1 < 0x90) return false; // overlong
            if (c == 0xF4 && c1 > 0x8F) return false; // beyond U+10FFFF
            p += 4;
            continue;
        }
        
        // Invalid leading byte
        return false;
    }
    
    return true;
}

} // namespace

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
    std::size_t count = 0;
    const std::size_t n = inputs.size();
    
    for (std::size_t i = 0; i < n; ++i) {
        const std::string& s = inputs[i];
        if (validate_utf8_fast(reinterpret_cast<const unsigned char*>(s.data()), s.size())) {
            ++count;
        }
    }
    
    return count;
}