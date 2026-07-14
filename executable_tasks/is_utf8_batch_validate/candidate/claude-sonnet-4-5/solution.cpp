#include "interface.h"
#include <cstdint>

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  // Lookup table for continuation byte count based on first byte
  static const uint8_t continuation_bytes[256] = {
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x00-0x0F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x10-0x1F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x20-0x2F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x30-0x3F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x40-0x4F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x50-0x5F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x60-0x6F
    0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0, // 0x70-0x7F
    9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 0x80-0x8F (invalid start)
    9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 0x90-0x9F (invalid start)
    9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 0xA0-0xAF (invalid start)
    9,9,9,9,9,9,9,9,9,9,9,9,9,9,9,9, // 0xB0-0xBF (invalid start)
    9,9,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 0xC0-0xCF (C0-C1 invalid, C2-CF need 1)
    1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1, // 0xD0-0xDF (need 1)
    2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, // 0xE0-0xEF (need 2)
    3,3,3,3,3,9,9,9,9,9,9,9,9,9,9,9  // 0xF0-0xFF (F0-F4 need 3, rest invalid)
  };

  auto valid_optimized = [&continuation_bytes](const std::string& s) -> bool {
    const uint8_t* p = reinterpret_cast<const uint8_t*>(s.data());
    const uint8_t* end = p + s.size();
    
    while (p < end) {
      uint8_t c = *p++;
      
      // Fast path for ASCII
      if (c <= 0x7F) continue;
      
      uint8_t extra = continuation_bytes[c];
      if (extra == 9) return false; // Invalid start byte
      
      // Check we have enough bytes
      if (end - p < extra) return false;
      
      // Validate continuation bytes and special ranges
      if (extra == 1) {
        if ((p[0] & 0xC0) != 0x80) return false;
        p += 1;
      } else if (extra == 2) {
        uint8_t b1 = p[0];
        uint8_t b2 = p[1];
        if ((b1 & 0xC0) != 0x80 || (b2 & 0xC0) != 0x80) return false;
        // Special range checks
        if (c == 0xE0 && b1 < 0xA0) return false; // Overlong
        if (c == 0xED && b1 > 0x9F) return false; // Surrogate
        p += 2;
      } else { // extra == 3
        uint8_t b1 = p[0];
        uint8_t b2 = p[1];
        uint8_t b3 = p[2];
        if ((b1 & 0xC0) != 0x80 || (b2 & 0xC0) != 0x80 || (b3 & 0xC0) != 0x80) return false;
        // Special range checks
        if (c == 0xF0 && b1 < 0x90) return false; // Overlong
        if (c == 0xF4 && b1 > 0x8F) return false; // Beyond U+10FFFF
        p += 3;
      }
    }
    return true;
  };

  std::size_t count = 0;
  for (const auto& s : inputs) {
    count += valid_optimized(s);
  }
  return count;
}
