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
    uint32_t q = v / 10u;
    *--p = static_cast<char>('0' + (v - q * 10u));
    v = q;
  } while (v != 0u);
  return dst + len;
}

static inline uint64_t fnv1a_append(uint64_t hash, const char* data, std::size_t len) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
  for (std::size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(p[i]);
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

  std::vector<uint32_t> ts_len(n);
  std::vector<uint32_t> id_len(n);
  std::vector<uint32_t> lvl_len(n);
  std::size_t total_len = 0;

  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t a = decimal_len_u32(timestamps[i]);
    const uint32_t b = decimal_len_u32(ids[i]);
    const uint32_t c = decimal_len_u32(static_cast<uint32_t>(levels[i]));
    ts_len[i] = a;
    id_len[i] = b;
    lvl_len[i] = c;
    total_len += static_cast<std::size_t>(a) + static_cast<std::size_t>(b) + static_cast<std::size_t>(c) + messages[i].size() + 4u;
  }

  std::string out;
  out.resize(total_len);

  for (int iter = 0; iter < iters; ++iter) {
    char* p = &out[0];
    for (std::size_t i = 0; i < n; ++i) {
      p = append_u32(p, timestamps[i]);
      *p++ = '|';
      p = append_u32(p, ids[i]);
      *p++ = '|';
      p = append_u32(p, static_cast<uint32_t>(levels[i]));
      *p++ = '|';

      const std::string& msg = messages[i];
      const std::size_t mlen = msg.size();
      if (mlen != 0) {
        const char* src = msg.data();
        for (std::size_t j = 0; j < mlen; ++j) p[j] = src[j];
        p += mlen;
      }
      *p++ = '\n';
    }

    uint64_t hash = 1469598103934665603ULL;
    hash = fnv1a_append(hash, out.data(), out.size());
    if (iter + 1 == iters) return hash;
  }

  return 0;
}
