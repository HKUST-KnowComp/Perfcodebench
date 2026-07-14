#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

static inline uint64_t fnv1a_update(uint64_t hash, const char* data, std::size_t len) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
  for (std::size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(p[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

static inline uint64_t fnv1a_update_char(uint64_t hash, unsigned char c) {
  hash ^= static_cast<uint64_t>(c);
  hash *= 1099511628211ULL;
  return hash;
}

static inline char* append_u32(char* dst, uint32_t v) {
  char buf[10];
  int n = 0;
  do {
    buf[n++] = static_cast<char>('0' + (v % 10));
    v /= 10;
  } while (v != 0);
  while (n--) {
    *dst++ = buf[n];
  }
  return dst;
}

static inline char* append_u16(char* dst, uint16_t v) {
  char buf[5];
  int n = 0;
  uint32_t x = v;
  do {
    buf[n++] = static_cast<char>('0' + (x % 10));
    x /= 10;
  } while (x != 0);
  while (n--) {
    *dst++ = buf[n];
  }
  return dst;
}

static inline std::size_t digits_u32(uint32_t v) {
  if (v >= 1000000000U) return 10;
  if (v >= 100000000U) return 9;
  if (v >= 10000000U) return 8;
  if (v >= 1000000U) return 7;
  if (v >= 100000U) return 6;
  if (v >= 10000U) return 5;
  if (v >= 1000U) return 4;
  if (v >= 100U) return 3;
  if (v >= 10U) return 2;
  return 1;
}

static inline std::size_t digits_u16(uint16_t v) {
  if (v >= 10000U) return 5;
  if (v >= 1000U) return 4;
  if (v >= 100U) return 3;
  if (v >= 10U) return 2;
  return 1;
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  const std::size_t n = ids.size();

  std::size_t total_len = 0;
  for (std::size_t i = 0; i < n; ++i) {
    total_len += digits_u32(timestamps[i]) + 1 + digits_u32(ids[i]) + 1 +
                 digits_u16(levels[i]) + 1 + messages[i].size() + 1;
  }

  std::string out;
  out.resize(total_len);

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

    uint64_t h = 1469598103934665603ULL;
    if (!out.empty()) {
      h = fnv1a_update(h, out.data(), out.size());
    }
    hash = h;
  }
  return hash;
}
