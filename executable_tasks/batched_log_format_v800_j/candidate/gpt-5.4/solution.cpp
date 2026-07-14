#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

static inline void fnv1a_update_byte(uint64_t& hash, unsigned char c) {
  hash ^= static_cast<uint64_t>(c);
  hash *= 1099511628211ULL;
}

static inline void fnv1a_update_buf(uint64_t& hash, const char* data, std::size_t len) {
  for (std::size_t i = 0; i < len; ++i) {
    fnv1a_update_byte(hash, static_cast<unsigned char>(data[i]));
  }
}

static inline unsigned decimal_len_u32(uint32_t v) {
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

static inline unsigned decimal_len_u16(uint16_t v) {
  if (v >= 10000u) return 5;
  if (v >= 1000u) return 4;
  if (v >= 100u) return 3;
  if (v >= 10u) return 2;
  return 1;
}

static inline char* append_u32_to(char* dst, uint32_t v) {
  unsigned len = decimal_len_u32(v);
  char* p = dst + len;
  do {
    *--p = static_cast<char>('0' + (v % 10));
    v /= 10;
  } while (v != 0);
  return dst + len;
}

static inline char* append_u16_to(char* dst, uint16_t v) {
  unsigned len = decimal_len_u16(v);
  char* p = dst + len;
  uint32_t x = v;
  do {
    *--p = static_cast<char>('0' + (x % 10));
    x /= 10;
  } while (x != 0);
  return dst + len;
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  const std::size_t n = ids.size();
  if (iters <= 0) return 0;

  std::vector<std::string> ts_strs(n);
  std::vector<std::string> id_strs(n);
  std::vector<std::string> lvl_strs(n);

  std::size_t total_size = 0;
  for (std::size_t i = 0; i < n; ++i) {
    ts_strs[i].resize(decimal_len_u32(timestamps[i]));
    append_u32_to(&ts_strs[i][0], timestamps[i]);

    id_strs[i].resize(decimal_len_u32(ids[i]));
    append_u32_to(&id_strs[i][0], ids[i]);

    lvl_strs[i].resize(decimal_len_u16(levels[i]));
    append_u16_to(&lvl_strs[i][0], levels[i]);

    total_size += ts_strs[i].size() + 1 + id_strs[i].size() + 1 + lvl_strs[i].size() + 1 + messages[i].size() + 1;
  }

  std::string out;
  out.reserve(total_size);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    for (std::size_t i = 0; i < n; ++i) {
      out.append(ts_strs[i]);
      out.push_back('|');
      out.append(id_strs[i]);
      out.push_back('|');
      out.append(lvl_strs[i]);
      out.push_back('|');
      out.append(messages[i]);
      out.push_back('\n');
    }

    uint64_t h = 1469598103934665603ULL;
    fnv1a_update_buf(h, out.data(), out.size());
    hash = h;
  }
  return hash;
}
