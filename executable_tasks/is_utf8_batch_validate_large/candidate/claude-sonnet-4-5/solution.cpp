#include "interface.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  // Lookup table for number of continuation bytes based on first byte
  // 0xFF means invalid start byte
  static constexpr unsigned char continuation_bytes[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x00-0x0F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x10-0x1F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x20-0x2F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x30-0x3F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x40-0x4F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x50-0x5F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x60-0x6F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x70-0x7F
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 0x80-0x8F (invalid)
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 0x90-0x9F (invalid)
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 0xA0-0xAF (invalid)
    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 0xB0-0xBF (invalid)
    0xFF,0xFF,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 0xC0-0xCF (C0-C1 invalid, C2-CF = 1 cont)
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 0xD0-0xDF (1 continuation)
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // 0xE0-0xEF (2 continuations)
    3,3,3,3,3,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF  // 0xF0-0xFF (F0-F4 = 3 cont, rest invalid)
  };

  auto valid_optimized = [&continuation_bytes](const std::string& s) -> bool {
    const unsigned char* p = reinterpret_cast<const unsigned char*>(s.data());
    const unsigned char* end = p + s.size();
    
    while (p < end) {
      unsigned char c = *p++;
      
      // Fast path for ASCII
      if (c <= 0x7F) continue;
      
      unsigned char cont_count = continuation_bytes[c];
      if (cont_count == 0xFF) return false;
      
      // Check we have enough bytes
      if (end - p < cont_count) return false;
      
      // Validate based on first byte and continuation bytes
      if (cont_count == 1) {
        // 2-byte sequence (C2-DF)
        if ((p[0] & 0xC0U) != 0x80U) return false;
        p += 1;
      } else if (cont_count == 2) {
        // 3-byte sequence (E0-EF)
        unsigned char b1 = p[0];
        unsigned char b2 = p[1];
        
        if ((b1 & 0xC0U) != 0x80U || (b2 & 0xC0U) != 0x80U) return false;
        
        // Special validation for overlong encodings and surrogates
        if (c == 0xE0 && b1 < 0xA0) return false;  // overlong
        if (c == 0xED && b1 > 0x9F) return false;  // surrogate
        
        p += 2;
      } else { // cont_count == 3
        // 4-byte sequence (F0-F4)
        unsigned char b1 = p[0];
        unsigned char b2 = p[1];
        unsigned char b3 = p[2];
        
        if ((b1 & 0xC0U) != 0x80U || (b2 & 0xC0U) != 0x80U || (b3 & 0xC0U) != 0x80U) return false;
        
        // Special validation for overlong and out-of-range
        if (c == 0xF0 && b1 < 0x90) return false;  // overlong
        if (c == 0xF4 && b1 > 0x8F) return false;  // > U+10FFFF
        
        p += 3;
      }
    }
    return true;
  };

  std::size_t count = 0;
  for (const auto& s : inputs) {
    count += valid_optimized(s) ? 1U : 0U;
  }
  return count;
}
