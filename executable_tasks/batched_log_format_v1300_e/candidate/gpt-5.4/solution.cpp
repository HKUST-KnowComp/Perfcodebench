#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

static inline uint64_t checksum_text(const std::string& text) {
  uint64_t hash = 1469598103934665603ULL;
  const unsigned char* p = reinterpret_cast<const unsigned char*>(text.data());
  const unsigned char* e = p + text.size();
  while (p != e) {
    hash ^= static_cast<uint64_t>(*p++);
    hash *= 1099511628211ULL;
  }
  return hash;
}

static inline unsigned decimal_length_u32(uint32_t v) {
  if (v >= 1000000000u) return 10;
  if (v >= 100000000u) return 9;
  if (v >= 10000000u) return 8;
  if (v >= 1000000u) return 7;
  if (v >= 100000u) return 6;
  if (v >= 10000u) return 5;
  if (v >= 1000u) return 4;
  if (v >= 100u) return 3;
  if (v >= 10u) return 2;
  return 1;
}

static inline unsigned decimal_length_u16(uint16_t v) {
  if (v >= 10000u) return 5;
  if (v >= 1000u) return 4;
  if (v >= 100u) return 3;
  if (v >= 10u) return 2;
  return 1;
}

static inline void append_u32(std::string& s, uint32_t v) {
  unsigned len = decimal_length_u32(v);
  const std::size_t old = s.size();
  s.resize(old + len);
  char* p = &s[old + len];
  do {
    *--p = static_cast<char>('0' + (v % 10u));
    v /= 10u;
  } while (v != 0);
}

static inline void append_u16(std::string& s, uint16_t v) {
  unsigned len = decimal_length_u16(v);
  const std::size_t old = s.size();
  s.resize(old + len);
  char* p = &s[old + len];
  uint32_t x = v;
  do {
    *--p = static_cast<char>('0' + (x % 10u));
    x /= 10u;
  } while (x != 0);
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  const std::size_t n = ids.size();

  std::vector<std::string> ts_text(n);
  std::vector<std::string> id_text(n);
  std::vector<std::string> lvl_text(n);
  std::size_t total_size = 0;

  for (std::size_t i = 0; i < n; ++i) {
    append_u32(ts_text[i], timestamps[i]);
    append_u32(id_text[i], ids[i]);
    append_u16(lvl_text[i], levels[i]);
    total_size += ts_text[i].size() + 1 + id_text[i].size() + 1 + lvl_text[i].size() +
                  1 + messages[i].size() + 1;
  }

  std::string out;
  out.reserve(total_size);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    for (std::size_t i = 0; i < n; ++i) {
      out += ts_text[i];
      out.push_back('|');
      out += id_text[i];
      out.push_back('|');
      out += lvl_text[i];
      out.push_back('|');
      out += messages[i];
      out.push_back('\n');
    }
    hash = checksum_text(out);
  }
  return hash;
}
