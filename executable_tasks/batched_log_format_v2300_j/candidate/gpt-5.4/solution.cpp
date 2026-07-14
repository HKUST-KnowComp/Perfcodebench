#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

static inline uint64_t fnv1a_append(uint64_t hash, const char* data, std::size_t len) {
  constexpr uint64_t kPrime = 1099511628211ULL;
  for (std::size_t i = 0; i < len; ++i) {
    hash ^= static_cast<unsigned char>(data[i]);
    hash *= kPrime;
  }
  return hash;
}

static inline uint64_t fnv1a_append_char(uint64_t hash, char c) {
  hash ^= static_cast<unsigned char>(c);
  hash *= 1099511628211ULL;
  return hash;
}

static inline char* append_u32_to_buffer(char* dst, uint32_t v) {
  char tmp[10];
  int n = 0;
  do {
    tmp[n++] = static_cast<char>('0' + (v % 10));
    v /= 10;
  } while (v != 0);
  while (n--) {
    *dst++ = tmp[n];
  }
  return dst;
}

static inline char* append_u16_to_buffer(char* dst, uint16_t v) {
  char tmp[5];
  int n = 0;
  do {
    tmp[n++] = static_cast<char>('0' + (v % 10));
    v = static_cast<uint16_t>(v / 10);
  } while (v != 0);
  while (n--) {
    *dst++ = tmp[n];
  }
  return dst;
}

static inline std::size_t digits_u32(uint32_t v) {
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

static inline std::size_t digits_u16(uint16_t v) {
  if (v >= 10000u) return 5;
  if (v >= 1000u) return 4;
  if (v >= 100u) return 3;
  if (v >= 10u) return 2;
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

  std::size_t total_size = 0;
  for (std::size_t i = 0; i < n; ++i) {
    total_size += digits_u32(timestamps[i]) + 1 + digits_u32(ids[i]) + 1 +
                  digits_u16(levels[i]) + 1 + messages[i].size() + 1;
  }

  std::string out;
  out.resize(total_size);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    char* p = out.empty() ? nullptr : &out[0];
    for (std::size_t i = 0; i < n; ++i) {
      p = append_u32_to_buffer(p, timestamps[i]);
      *p++ = '|';
      p = append_u32_to_buffer(p, ids[i]);
      *p++ = '|';
      p = append_u16_to_buffer(p, levels[i]);
      *p++ = '|';

      const std::string& msg = messages[i];
      const std::size_t msz = msg.size();
      if (msz != 0) {
        __builtin_memcpy(p, msg.data(), msz);
        p += msz;
      }
      *p++ = '\n';
    }

    uint64_t h = 1469598103934665603ULL;
    if (total_size != 0) {
      h = fnv1a_append(h, out.data(), total_size);
    }
    hash = h;
  }
  return hash;
}
