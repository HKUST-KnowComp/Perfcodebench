#include "interface.h"
#include "is_utf8.h"
#include <cstring>

std::size_t count_valid_lines(const std::string& input) {
  std::size_t count = 0;
  const char* p = input.data();
  const char* const end = p + input.size();
  
  while (p < end) {
    const char* line_start = p;
    const char* line_end = static_cast<const char*>(std::memchr(p, '\n', end - p));
    
    if (!line_end) {
      line_end = end;
    }
    
    std::size_t line_len = line_end - line_start;
    
    // Fast path: check if line is pure ASCII
    bool is_ascii = true;
    const char* scan = line_start;
    const char* scan_end = line_end;
    
    // Process 8 bytes at a time for ASCII check
    while (scan + 8 <= scan_end) {
      std::uint64_t chunk;
      std::memcpy(&chunk, scan, 8);
      if (chunk & 0x8080808080808080ULL) {
        is_ascii = false;
        break;
      }
      scan += 8;
    }
    
    // Check remaining bytes
    if (is_ascii) {
      while (scan < scan_end) {
        if (static_cast<unsigned char>(*scan) & 0x80) {
          is_ascii = false;
          break;
        }
        ++scan;
      }
    }
    
    if (is_ascii) {
      ++count;
    } else {
      // Use SIMD validation from is_utf8.h
      if (is_utf8(line_start, line_len)) {
        ++count;
      }
    }
    
    p = (line_end < end) ? (line_end + 1) : end;
  }
  
  return count;
}