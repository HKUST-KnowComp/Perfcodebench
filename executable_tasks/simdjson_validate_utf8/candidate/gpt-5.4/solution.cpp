#include "interface.h"
#include "simdjson.h"

#include <cstddef>
#include <string>
#include <vector>

namespace {

static inline bool is_ascii(const std::string &s) noexcept {
  const unsigned char *p = reinterpret_cast<const unsigned char *>(s.data());
  size_t n = s.size();
  size_t i = 0;

  uint64_t orv = 0;
  for (; i + 8 <= n; i += 8) {
    uint64_t v;
    __builtin_memcpy(&v, p + i, sizeof(v));
    orv |= v;
  }
  if ((orv & 0x8080808080808080ULL) != 0) return false;
  for (; i < n; ++i) {
    if (p[i] & 0x80U) return false;
  }
  return true;
}

static inline bool is_valid_utf8_fallback(const std::string& s) noexcept {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(s.data());
  const unsigned char* end = p + s.size();
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
      if (end - p < 2 || p[0] < 0x80 || p[0] > 0x9F || (p[1] & 0xC0U) != 0x80U) return false;
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
      if (end - p < 3 || p[0] < 0x80 || p[0] > 0x8F || (p[1] & 0xC0U) != 0x80U || (p[2] & 0xC0U) != 0x80U) return false;
      p += 3;
      continue;
    }
    return false;
  }
  return true;
}

static inline bool is_valid_utf8_simdjson(const std::string& s) noexcept {
  if (s.empty() || is_ascii(s)) return true;
  return simdjson::validate_utf8(s.data(), s.size());
}

}  // namespace

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const auto& s : inputs) {
    count += is_valid_utf8_simdjson(s) ? 1u : 0u;
  }
  return count;
}
