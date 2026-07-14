#include "is_utf8.h"
#include <cstddef>
#include <string>
#include <vector>

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  auto valid_scalar = [](const std::string& s) -> bool {
    const unsigned char* p = reinterpret_cast<const unsigned char*>(s.data());
    const unsigned char* end = p + s.size();
    
    while (p < end) {
      const unsigned char c = *p;
      
      // ASCII byte
      if (c <= 0x7F) {
        ++p;
        continue;
      }
      
      // Invalid continuation byte or overlong
      if ((c & 0xC0U) == 0x80U || c < 0xC2) return false;
      
      // 2-byte sequence
      if ((c & 0xE0U) == 0xC0U) {
        if (p + 1 >= end || (p[1] & 0xC0U) != 0x80U) return false;
        p += 2;
        continue;
      }
      
      // 3-byte sequence
      if ((c & 0xF0U) == 0xE0U) {
        if (p + 2 >= end) return false;
        if ((p[1] & 0xC0U) != 0x80U || (p[2] & 0xC0U) != 0x80U) return false;
        // Check for overlong and surrogates
        if (c == 0xE0 && p[1] < 0xA0) return false;
        if (c == 0xED && p[1] >= 0xA0) return false;
        p += 3;
        continue;
      }
      
      // 4-byte sequence
      if ((c & 0xF8U) == 0xF0U) {
        if (p + 3 >= end) return false;
        if ((p[1] & 0xC0U) != 0x80U || (p[2] & 0xC0U) != 0x80U || (p[3] & 0xC0U) != 0x80U) return false;
        // Check for overlong and out-of-range
        if (c == 0xF0 && p[1] < 0x90) return false;
        if (c == 0xF4 && p[1] > 0x8F) return false;
        p += 4;
        continue;
      }
      
      // Invalid leading byte
      return false;
    }
    return true;
  };

  std::size_t count = 0;
  for (const auto& s : inputs) {
    count += valid_scalar(s) ? 1U : 0U;
  }
  return count;
}