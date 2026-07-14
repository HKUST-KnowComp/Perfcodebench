#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>
#include <cstring>

namespace {

inline uint64_t checksum_update(uint64_t hash, const void* data, std::size_t len) {
  const unsigned char* p = static_cast<const unsigned char*>(data);
  const unsigned char* end = p + len;
  while (p != end) {
    hash ^= static_cast<uint64_t>(*p++);
    hash *= 1099511628211ULL;
  }
  return hash;
}

// write unsigned decimal into buf, return advanced pointer
inline char* u32toa(char* buf, uint32_t v) {
  if (v == 0) { *buf++ = '0'; return buf; }
  char tmp[16];
  char* p = tmp;
  while (v) { *p++ = char('0' + (v % 10)); v /= 10; }
  while (p != tmp) *buf++ = *--p;
  return buf;
}

inline char* u16toa(char* buf, uint16_t v) {
  return u32toa(buf, v);
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  const std::size_t n = ids.size();
  // rough upper-bound for one iteration
  std::size_t cap = 0;
  for (std::size_t i = 0; i < n; ++i) {
    cap += 32 + messages[i].size();  // 32 chars enough for ints + pipes + newline
  }
  std::string buf;
  buf.reserve(cap + 1);

  uint64_t hash = 1469598103934665603ULL;
  for (int iter = 0; iter < iters; ++iter) {
    buf.clear();
    char* base = buf.data();
    std::size_t len = 0;
    for (std::size_t i = 0; i < n; ++i) {
      char tmp[64];
      char* p = tmp;
      p = u32toa(p, timestamps[i]); *p++ = '|';
      p = u32toa(p, ids[i]);        *p++ = '|';
      p = u16toa(p, levels[i]);     *p++ = '|';
      std::size_t part = p - tmp;
      std::size_t mlen = messages[i].size();
      std::size_t need = len + part + mlen + 1;  // +1 for '\n'
      if (need > buf.capacity()) {
        buf.resize(need * 2);
        base = buf.data();
      }
      std::memcpy(base + len, tmp, part);
      len += part;
      std::memcpy(base + len, messages[i].data(), mlen);
      len += mlen;
      base[len++] = '\n';
    }
    hash = checksum_update(hash, base, len);
  }
  return hash;
}