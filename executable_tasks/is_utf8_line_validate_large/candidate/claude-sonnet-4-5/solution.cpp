#include "interface.h"

#include <cstddef>
#include <string>

namespace {

inline bool is_valid_utf8_inplace(const unsigned char* p, const unsigned char* const end) {
  while (p < end) {
    const unsigned char c = *p++;
    
    // ASCII fast path
    if (c <= 0x7F) continue;
    
    // 2-byte sequence
    if (c >= 0xC2 && c <= 0xDF) {
      if (p >= end || (*p & 0xC0U) != 0x80U) return false;
      ++p;
      continue;
    }
    
    // 3-byte sequences
    if (c >= 0xE0 && c <= 0xEF) {
      if (end - p < 2) return false;
      
      if (c == 0xE0) {
        if (p[0] < 0xA0 || p[0] > 0xBF || (p[1] & 0xC0U) != 0x80U) return false;
      } else if (c == 0xED) {
        if (p[0] < 0x80 || p[0] > 0x9FU || (p[1] & 0xC0U) != 0x80U) return false;
      } else {
        if ((p[0] & 0xC0U) != 0x80U || (p[1] & 0xC0U) != 0x80U) return false;
      }
      p += 2;
      continue;
    }
    
    // 4-byte sequences
    if (c >= 0xF0 && c <= 0xF4) {
      if (end - p < 3) return false;
      
      if (c == 0xF0) {
        if (p[0] < 0x90 || p[0] > 0xBF || (p[1] & 0xC0U) != 0x80U || (p[2] & 0xC0U) != 0x80U) return false;
      } else if (c == 0xF4) {
        if (p[0] < 0x80 || p[0] > 0x8FU || (p[1] & 0xC0U) != 0x80U || (p[2] & 0xC0U) != 0x80U) return false;
      } else {
        if ((p[0] & 0xC0U) != 0x80U || (p[1] & 0xC0U) != 0x80U || (p[2] & 0xC0U) != 0x80U) return false;
      }
      p += 3;
      continue;
    }
    
    return false;
  }
  return true;
}

}  // namespace

std::size_t count_valid_lines(const std::string& input) {
  std::size_t count = 0;
  const unsigned char* p = reinterpret_cast<const unsigned char*>(input.data());
  const unsigned char* const end = p + input.size();
  const unsigned char* line_start = p;
  
  while (p < end) {
    if (*p == '\n') {
      count += is_valid_utf8_inplace(line_start, p) ? 1U : 0U;
      ++p;
      line_start = p;
    } else {
      ++p;
    }
  }
  
  // Handle last line if input doesn't end with newline
  if (line_start < end) {
    count += is_valid_utf8_inplace(line_start, end) ? 1U : 0U;
  }
  
  return count;
}
