#include "interface.h"
#include <cstdint>
#include <cstring>

#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)
#include <immintrin.h>
#define HAS_SSE2 1
#if defined(__SSE4_2__) || defined(__AVX2__)
#define HAS_SSE42 1
#endif
#if defined(__AVX2__)
#define HAS_AVX2 1
#endif
#endif

namespace {

inline bool validate_utf8_scalar(const unsigned char* p, const unsigned char* end) {
    while (p < end) {
        const unsigned char c = *p++;
        if (c <= 0x7F) continue;
        if (c >= 0xC2 && c <= 0xDF) {
            if (p >= end || (p[0] & 0xC0U) != 0x80U) return false;
            ++p;
            continue;
        }
        if (c == 0xE0) {
            if (end - p < 2 || p[0] < 0xA0 || p[0] > 0xBF || (p[1] & 0xC0U) != 0x80U) return false;
            p += 2;
            continue;
        }
        if (c >= 0xE1 && c <= 0xEC) {
            if (end - p < 2 || (p[0] & 0xC0U) != 0x80U || (p[1] & 0xC0U) != 0x80U) return false;
            p += 2;
            continue;
        }
        if (c == 0xED) {
            if (end - p < 2 || p[0] < 0x80 || p[0] > 0x9FU || (p[1] & 0xC0U) != 0x80U) return false;
            p += 2;
            continue;
        }
        if (c >= 0xEE && c <= 0xEF) {
            if (end - p < 2 || (p[0] & 0xC0U) != 0x80U || (p[1] & 0xC0U) != 0x80U) return false;
            p += 2;
            continue;
        }
        if (c == 0xF0) {
            if (end - p < 3 || p[0] < 0x90 || p[0] > 0xBF || (p[1] & 0xC0U) != 0x80U || (p[2] & 0xC0U) != 0x80U) return false;
            p += 3;
            continue;
        }
        if (c >= 0xF1 && c <= 0xF3) {
            if (end - p < 3 || (p[0] & 0xC0U) != 0x80U || (p[1] & 0xC0U) != 0x80U || (p[2] & 0xC0U) != 0x80U) return false;
            p += 3;
            continue;
        }
        if (c == 0xF4) {
            if (end - p < 3 || p[0] < 0x80 || p[0] > 0x8FU || (p[1] & 0xC0U) != 0x80U || (p[2] & 0xC0U) != 0x80U) return false;
            p += 3;
            continue;
        }
        return false;
    }
    return true;
}

#ifdef HAS_AVX2
inline bool validate_utf8_avx2(const unsigned char* data, size_t len) {
    const unsigned char* p = data;
    const unsigned char* end = data + len;
    
    const __m256i high_bit = _mm256_set1_epi8(static_cast<char>(0x80));
    
    while (p + 32 <= end) {
        __m256i chunk = _mm256_loadu_si256(reinterpret_cast<const __m256i*>(p));
        __m256i has_high = _mm256_and_si256(chunk, high_bit);
        if (_mm256_testz_si256(has_high, has_high)) {
            p += 32;
            continue;
        }
        break;
    }
    
    return validate_utf8_scalar(p, end);
}
#endif

#ifdef HAS_SSE2
inline bool validate_utf8_sse2(const unsigned char* data, size_t len) {
    const unsigned char* p = data;
    const unsigned char* end = data + len;
    
    const __m128i high_bit = _mm_set1_epi8(static_cast<char>(0x80));
    
    while (p + 16 <= end) {
        __m128i chunk = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
        int mask = _mm_movemask_epi8(chunk);
        if (mask == 0) {
            p += 16;
            continue;
        }
        break;
    }
    
    return validate_utf8_scalar(p, end);
}
#endif

inline bool validate_utf8_fast(const unsigned char* data, size_t len) {
    if (len == 0) return true;
    
#ifdef HAS_AVX2
    return validate_utf8_avx2(data, len);
#elif defined(HAS_SSE2)
    return validate_utf8_sse2(data, len);
#else
    return validate_utf8_scalar(data, data + len);
#endif
}

} // namespace

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
    std::size_t count = 0;
    const size_t n = inputs.size();
    
    for (size_t i = 0; i < n; ++i) {
        const std::string& s = inputs[i];
        if (validate_utf8_fast(reinterpret_cast<const unsigned char*>(s.data()), s.size())) {
            ++count;
        }
    }
    
    return count;
}