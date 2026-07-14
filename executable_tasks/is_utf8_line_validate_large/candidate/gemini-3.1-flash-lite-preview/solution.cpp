#include "interface.h"
#include <string>
#include <string_view>

namespace {

// Validates UTF-8 in-place without allocations.
// Returns true if the range [p, end) is valid UTF-8.
inline bool is_valid_utf8_range(const unsigned char* p, const unsigned char* end) {
  while (p < end) {
    const unsigned char c = *p++;
    if (c <= 0x7F) continue;
    if (c >= 0xC2 && c <= 0xDF) {
      if (p >= end || (p[0] & 0xC0U) != 0x80U) return false;
      ++p;
    } else if (c >= 0xE0 && c <= 0xEF) {
      if (end - p < 2) return false;
      unsigned char b1 = p[0], b2 = p[1];
      if ((b1 & 0xC0U) != 0x80U || (b2 & 0xC0U) != 0x80U) return false;
      if (c == 0xE0 && b1 < 0xA0) return false;
      if (c == 0xED && b1 > 0x9F) return false;
      p += 2;
    } else if (c >= 0xF0 && c <= 0xF4) {
      if (end - p < 3) return false;
      unsigned char b1 = p[0], b2 = p[1], b3 = p[2];
      if ((b1 & 0xC0U) != 0x80U || (b2 & 0xC0U) != 0x80U || (b3 & 0xC0U) != 0x80U) return false;
      if (c == 0xF0 && b1 < 0x90) return false;
      if (c == 0xF4 && b1 > 0x8F) return false;
      p += 3;
    } else {
      return false;
    }
  }
  return true;
}

}  // namespace

std::size_t count_valid_lines(const std::string& input) {
  if (input.empty()) return 0;
  std::size_t count = 0;
  const unsigned char* data = reinterpret_cast<const unsigned char*>(input.data());
  const unsigned char* end = data + input.size();
  const unsigned char* start = data;

  while (start < end) {
    const unsigned char* next = start;
    while (next < end && *next != '\n') {
      ++next;
    }
    if (is_valid_utf8_range(start, next)) {
      count++;
    }
    start = next + 1;
  }
  
  // Handle trailing newline case if necessary based on problem definition
  if (!input.empty() && input.back() == '\n') {
      // The loop above counts the empty string after the last newline as a line.
      // If the requirement is to count lines delimited by \n, this logic holds.
  }

  return count;
}