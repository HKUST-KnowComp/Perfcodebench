#include "interface.h"

#include <vector>

namespace {

uint64_t checksum_utf16(const std::vector<char16_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (char16_t ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t transcode_checksum(const std::string& input) {
  std::vector<char16_t> out;
  out.reserve(input.size());
  const unsigned char* p = reinterpret_cast<const unsigned char*>(input.data());
  const unsigned char* end = p + input.size();
  while (p < end) {
    uint32_t cp = 0;
    const unsigned char c = *p++;
    if (c <= 0x7F) {
      cp = c;
    } else if ((c >> 5) == 0x6) {
      cp = (static_cast<uint32_t>(c & 0x1FU) << 6) |
           static_cast<uint32_t>(*p++ & 0x3FU);
    } else if ((c >> 4) == 0xE) {
      cp = (static_cast<uint32_t>(c & 0x0FU) << 12) |
           (static_cast<uint32_t>(p[0] & 0x3FU) << 6) |
           static_cast<uint32_t>(p[1] & 0x3FU);
      p += 2;
    } else {
      cp = (static_cast<uint32_t>(c & 0x07U) << 18) |
           (static_cast<uint32_t>(p[0] & 0x3FU) << 12) |
           (static_cast<uint32_t>(p[1] & 0x3FU) << 6) |
           static_cast<uint32_t>(p[2] & 0x3FU);
      p += 3;
    }
    if (cp <= 0xFFFFU) {
      out.push_back(static_cast<char16_t>(cp));
    } else {
      cp -= 0x10000U;
      out.push_back(static_cast<char16_t>(0xD800U + (cp >> 10)));
      out.push_back(static_cast<char16_t>(0xDC00U + (cp & 0x3FFU)));
    }
  }
  return checksum_utf16(out);
}
