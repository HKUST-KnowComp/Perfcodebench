#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

static inline std::size_t decimal_len_u32(uint32_t v) {
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

static inline std::size_t decimal_len_u16(uint16_t v) {
  if (v >= 10000u) return 5;
  if (v >= 1000u) return 4;
  if (v >= 100u) return 3;
  if (v >= 10u) return 2;
  return 1;
}

static inline char* append_u32(char* p, uint32_t v) {
  char buf[10];
  int n = 0;
  do {
    buf[n++] = static_cast<char>('0' + (v % 10u));
    v /= 10u;
  } while (v != 0u);
  while (n--) {
    *p++ = buf[n];
  }
  return p;
}

static inline char* append_u16(char* p, uint16_t v) {
  char buf[5];
  int n = 0;
  uint32_t x = v;
  do {
    buf[n++] = static_cast<char>('0' + (x % 10u));
    x /= 10u;
  } while (x != 0u);
  while (n--) {
    *p++ = buf[n];
  }
  return p;
}

static inline uint64_t fnv1a_bytes(const char* data, std::size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i < len; ++i) {
    hash ^= static_cast<unsigned char>(data[i]);
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

  std::size_t total_size = 0;
  for (std::size_t i = 0; i < n; ++i) {
    total_size += decimal_len_u32(timestamps[i]) + 1 +
                  decimal_len_u32(ids[i]) + 1 +
                  decimal_len_u16(levels[i]) + 1 +
                  messages[i].size() + 1;
  }

  std::string out;
  out.resize(total_size);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    char* p = out.empty() ? nullptr : &out[0];
    for (std::size_t i = 0; i < n; ++i) {
      p = append_u32(p, timestamps[i]);
      *p++ = '|';
      p = append_u32(p, ids[i]);
      *p++ = '|';
      p = append_u16(p, levels[i]);
      *p++ = '|';

      const std::string& msg = messages[i];
      const std::size_t mlen = msg.size();
      if (mlen != 0) {
        __builtin_memcpy(p, msg.data(), mlen);
        p += mlen;
      }
      *p++ = '\n';
    }
    hash = fnv1a_bytes(out.data(), out.size());
  }
  return hash;
}
