#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

static inline uint32_t decimal_len_u32(uint32_t v) {
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

static inline char* append_u32(char* dst, uint32_t v) {
  uint32_t len = decimal_len_u32(v);
  char* p = dst + len;
  do {
    *--p = static_cast<char>('0' + (v % 10u));
    v /= 10u;
  } while (v != 0u);
  return dst + len;
}

static inline char* append_u16(char* dst, uint16_t v) {
  return append_u32(dst, static_cast<uint32_t>(v));
}

static inline uint64_t fnv1a_update(uint64_t hash, const char* data, std::size_t len) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
  const unsigned char* end = p + len;
  while (p != end) {
    hash ^= static_cast<uint64_t>(*p++);
    hash *= 1099511628211ULL;
  }
  return hash;
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

  std::size_t total_size = 0;
  for (std::size_t i = 0; i < n; ++i) {
    total_size += decimal_len_u32(timestamps[i]);
    total_size += 1;
    total_size += decimal_len_u32(ids[i]);
    total_size += 1;
    total_size += decimal_len_u32(static_cast<uint32_t>(levels[i]));
    total_size += 1;
    total_size += messages[i].size();
    total_size += 1;
  }

  std::string out;
  out.resize(total_size);

  for (int iter = 0; iter < iters; ++iter) {
    char* p = &out[0];
    for (std::size_t i = 0; i < n; ++i) {
      p = append_u32(p, timestamps[i]);
      *p++ = '|';
      p = append_u32(p, ids[i]);
      *p++ = '|';
      p = append_u16(p, levels[i]);
      *p++ = '|';

      const std::string& msg = messages[i];
      const std::size_t msz = msg.size();
      if (msz != 0) {
        __builtin_memcpy(p, msg.data(), msz);
        p += msz;
      }
      *p++ = '\n';
    }
  }

  return fnv1a_update(1469598103934665603ULL, out.data(), out.size());
}
