#include "interface.h"

std::size_t count_valid_lines(const std::string& input) {
  auto valid_scalar = [](const char* data, std::size_t len) -> bool {
    const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
    const unsigned char* end = p + len;
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
  };

  std::size_t count = 0;
  const char* p = input.data();
  const char* const end = p + input.size();
  while (p < end) {
    const char* line_end = p;
    while (line_end < end && *line_end != '\n') {
      ++line_end;
    }
    count += valid_scalar(p, static_cast<std::size_t>(line_end - p)) ? 1U : 0U;
    p = (line_end < end) ? (line_end + 1) : line_end;
  }
  return count;
}
