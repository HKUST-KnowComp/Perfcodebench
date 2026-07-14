#include "interface.h"
#include "is_utf8.h"
#include <immintrin.h>

static inline bool validate_utf8_chunk(const unsigned char* data, std::size_t len) {
  if (len == 0) return true;
  
  const unsigned char* p = data;
  const unsigned char* end = p + len;
  
  // Process 16 bytes at a time using SIMD
  while (p + 16 <= end) {
    __m128i v = _mm_loadu_si128(reinterpret_cast<const __m128i*>(p));
    
    // Check for ASCII (0x00-0x7F) - all bytes should be < 0x80
    __m128i ascii_mask = _mm_cmplt_epi8(v, _mm_set1_epi8(0x80));
    __m128i ascii_count = _mm_popcnt_epi8(ascii_mask);
    
    // Check for continuation bytes (0x80-0xBF)
    __m128i cont_mask = _mm_and_si128(
      _mm_cmpgt_epi8(v, _mm_set1_epi8(0x7F)),
      _mm_cmplt_epi8(v, _mm_set1_epi8(0xC0))
    );
    
    // Check for invalid bytes (0xC0-0xC1, 0xF5-0xFF)
    __m128i invalid_mask = _mm_or_si128(
      _mm_and_si128(
        _mm_cmpgt_epi8(v, _mm_set1_epi8(0xBF)),
        _mm_cmplt_epi8(v, _mm_set1_epi8(0xC2))
      ),
      _mm_and_si128(
        _mm_cmpgt_epi8(v, _mm_set1_epi8(0xF4)),
        _mm_cmplt_epi8(v, _mm_set1_epi8(0x00))
      )
    );
    
    // If any invalid bytes found, return false
    if (_mm_movemask_epi8(invalid_mask) != 0) {
      return false;
    }
    
    p += 16;
  }
  
  // Handle remaining bytes
  while (p < end) {
    unsigned char c = *p++;
    if (c <= 0x7F) continue;
    if (c >= 0xC2 && c <= 0xDF) {
      if (p >= end || (*p & 0xC0U) != 0x80U) return false;
      ++p;
      continue;
    }
    if (c == 0xE0) {
      if (end - p < 2 || *p < 0xA0 || *p > 0xBF || (*p & 0xC0U) != 0x80U) return false;
      p += 2;
      continue;
    }
    if (c >= 0xE1 && c <= 0xEC) {
      if (end - p < 2 || (*p & 0xC0U) != 0x80U || (*(p+1) & 0xC0U) != 0x80U) return false;
      p += 2;
      continue;
    }
    if (c == 0xED) {
      if (end - p < 2 || *p < 0x80 || *p > 0x9F || (*(p+1) & 0xC0U) != 0x80U) return false;
      p += 2;
      continue;
    }
    if (c >= 0xEE && c <= 0xEF) {
      if (end - p < 2 || (*p & 0xC0U) != 0x80U || (*(p+1) & 0xC0U) != 0x80U) return false;
      p += 2;
      continue;
    }
    if (c == 0xF0) {
      if (end - p < 3 || *p < 0x90 || *p > 0xBF || (*(p+1) & 0xC0U) != 0x80U || (*(p+2) & 0xC0U) != 0x80U) return false;
      p += 3;
      continue;
    }
    if (c >= 0xF1 && c <= 0xF3) {
      if (end - p < 3 || (*p & 0xC0U) != 0x80U || (*(p+1) & 0xC0U) != 0x80U || (*(p+2) & 0xC0U) != 0x80U) return false;
      p += 3;
      continue;
    }
    if (c == 0xF4) {
      if (end - p < 3 || *p < 0x80 || *p > 0x8F || (*(p+1) & 0xC0U) != 0x80U || (*(p+2) & 0xC0U) != 0x80U) return false;
      p += 3;
      continue;
    }
    return false;
  }
  
  return true;
}

std::size_t count_valid_lines(const std::string& input) {
  std::size_t count = 0;
  const char* p = input.data();
  const char* const end = p + input.size();
  
  while (p < end) {
    const char* line_start = p;
    const char* line_end = p;
    
    // Find end of line
    while (line_end < end && *line_end != '\n') {
      ++line_end;
    }
    
    // Validate line
    if (validate_utf8_chunk(reinterpret_cast<const unsigned char*>(line_start), 
                            static_cast<std::size_t>(line_end - line_start))) {
      ++count;
    }
    
    // Move to next line
    p = (line_end < end) ? (line_end + 1) : line_end;
  }
  
  return count;
}