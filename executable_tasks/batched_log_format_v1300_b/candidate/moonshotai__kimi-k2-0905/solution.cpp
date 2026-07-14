#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline uint64_t checksum_buf(const char* first, const char* last) {
  uint64_t hash = 1469598103934665603ULL;
  for (; first != last; ++first) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(*first));
    hash *= 1099511628211ULL;
  }
  return hash;
}

// Write integer into buf, return pointer past the last digit.
inline char* u32toa(uint32_t val, char* buf) {
  if (val == 0) { *buf++ = '0'; return buf; }
  char tmp[10];
  char* p = tmp + 10;
  while (val) {
    *--p = '0' + (val % 10);
    val /= 10;
  }
  while (p != tmp + 10) *buf++ = *p++;
  return buf;
}

inline char* u16toa(uint16_t val, char* buf) {
  return u32toa(static_cast<uint32_t>(val), buf);
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  const size_t n = ids.size();

  // conservative upper-bound for one line
  const size_t max_line = 10 + 1 + 10 + 1 + 5 + 1 + 1024 + 1;
  const size_t total = n * max_line;
  std::string out;
  out.reserve(total);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    char* ptr = out.data() + out.size();
    for (size_t i = 0; i < n; ++i) {
      ptr = u32toa(timestamps[i], ptr);
      *ptr++ = '|';
      ptr = u32toa(ids[i], ptr);
      *ptr++ = '|';
      ptr = u16toa(levels[i], ptr);
      *ptr++ = '|';
      const std::string& msg = messages[i];
      memcpy(ptr, msg.data(), msg.size());
      ptr += msg.size();
      *ptr++ = '\n';
    }
    out.resize(ptr - out.data());
    hash = checksum_buf(out.data(), out.data() + out.size());
  }
  return hash;
}