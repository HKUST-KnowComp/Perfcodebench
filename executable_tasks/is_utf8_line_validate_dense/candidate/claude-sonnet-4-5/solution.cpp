#include "interface.h"

std::size_t count_valid_lines(const std::string& input) {
  auto valid_scalar = [](const char* data, std::size_t len) -> bool {
    const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
    const unsigned char* end = p + len;
    
    while (p < end) {
      unsigned char c = *p++;
      
      // ASCII fast path
      if (c <= 0x7F) continue;
      
      // Determine sequence length and validate first byte
      int extra_bytes;
      unsigned char min_second, max_second;
      
      if (c >= 0xC2 && c <= 0xDF) {
        extra_bytes = 1;
        min_second = 0x80;
        max_second = 0xBF;
      } else if (c == 0xE0) {
        extra_bytes = 2;
        min_second = 0xA0;
        max_second = 0xBF;
      } else if (c >= 0xE1 && c <= 0xEC) {
        extra_bytes = 2;
        min_second = 0x80;
        max_second = 0xBF;
      } else if (c == 0xED) {
        extra_bytes = 2;
        min_second = 0x80;
        max_second = 0x9F;
      } else if (c >= 0xEE && c <= 0xEF) {
        extra_bytes = 2;
        min_second = 0x80;
        max_second = 0xBF;
      } else if (c == 0xF0) {
        extra_bytes = 3;
        min_second = 0x90;
        max_second = 0xBF;
      } else if (c >= 0xF1 && c <= 0xF3) {
        extra_bytes = 3;
        min_second = 0x80;
        max_second = 0xBF;
      } else if (c == 0xF4) {
        extra_bytes = 3;
        min_second = 0x80;
        max_second = 0x8F;
      } else {
        return false;
      }
      
      // Check bounds once
      if (end - p < extra_bytes) return false;
      
      // Validate second byte with range check
      unsigned char second = *p++;
      if (second < min_second || second > max_second) return false;
      
      // Validate remaining continuation bytes (branchless)
      for (int i = 1; i < extra_bytes; ++i) {
        if ((*p++ & 0xC0) != 0x80) return false;
      }
    }
    return true;
  };

  std::size_t count = 0;
  const char* p = input.data();
  const char* const end = p + input.size();
  
  while (p < end) {
    const char* line_end = p;
    // Find newline
    while (line_end < end && *line_end != '\n') {
      ++line_end;
    }
    
    // Validate and count
    count += valid_scalar(p, static_cast<std::size_t>(line_end - p));
    
    // Move to next line
    p = (line_end < end) ? (line_end + 1) : line_end;
  }
  
  return count;
}
