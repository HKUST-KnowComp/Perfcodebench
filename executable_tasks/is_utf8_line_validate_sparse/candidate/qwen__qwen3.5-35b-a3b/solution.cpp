#include "is_utf8.h"
#include <cstddef>
#include <string>

namespace {

// Classify byte: 0=ASCII, 1=2-byte lead, 2=3-byte lead, 3=4-byte lead, 4=invalid lead, 5=continuation
inline unsigned char classify_byte(unsigned char c) {
  if (c <= 0x7F) return 0;
  if (c >= 0xC2 && c <= 0xDF) return 1;
  if (c >= 0xE0 && c <= 0xEF) return 2;
  if (c >= 0xF0 && c <= 0xF4) return 3;
  return 4;
}

// Check if byte is valid continuation (10xxxxxx)
inline bool is_continuation(unsigned char c) {
  return (c & 0xC0U) == 0x80U;
}

// Validate UTF-8 sequence starting at p with length len
inline bool validate_utf8(const unsigned char* p, std::size_t len) {
  const unsigned char* end = p + len;
  while (p < end) {
    unsigned char c = *p++;
    
    if (c <= 0x7F) continue;
    
    // 2-byte sequence
    if (c >= 0xC2 && c <= 0xDF) {
      if (p >= end || !is_continuation(*p)) return false;
      ++p;
      continue;
    }
    
    // 3-byte sequence
    if (c >= 0xE0 && c <= 0xEF) {
      if (end - p < 2) return false;
      unsigned char c1 = *p++, c2 = *p++;
      
      if (c == 0xE0) {
        if (c1 < 0xA0 || !is_continuation(c1) || !is_continuation(c2)) return false;
      } else if (c == 0xED) {
        if (c1 > 0x9F || !is_continuation(c1) || !is_continuation(c2)) return false;
      } else {
        if (!is_continuation(c1) || !is_continuation(c2)) return false;
      }
      continue;
    }
    
    // 4-byte sequence
    if (c >= 0xF0 && c <= 0xF4) {
      if (end - p < 3) return false;
      unsigned char c1 = *p++, c2 = *p++, c3 = *p++;
      
      if (c == 0xF0) {
        if (c1 < 0x90 || !is_continuation(c1) || !is_continuation(c2) || !is_continuation(c3)) return false;
      } else if (c == 0xF4) {
        if (c1 > 0x8F || !is_continuation(c1) || !is_continuation(c2) || !is_continuation(c3)) return false;
      } else {
        if (!is_continuation(c1) || !is_continuation(c2) || !is_continuation(c3)) return false;
      }
      continue;
    }
    
    return false;
  }
  return true;
}

} // namespace

std::size_t count_valid_lines(const std::string& input) {
  std::size_t count = 0;
  const char* p = input.data();
  const char* const end = p + input.size();
  
  while (p < end) {
    const char* line_end = p;
    while (line_end < end && *line_end != '\n') {
      ++line_end;
    }
    
    if (validate_utf8(reinterpret_cast<const unsigned char*>(p), 
                      static_cast<std::size_t>(line_end - p))) {
      ++count;
    }
    
    p = (line_end < end) ? (line_end + 1) : line_end;
  }
  
  return count;
}