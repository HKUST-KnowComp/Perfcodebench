#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

namespace {

inline uint64_t checksum_text(const std::string& text) {
  const unsigned char* data = reinterpret_cast<const unsigned char*>(text.data());
  const size_t len = text.size();
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(data[i]);
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
  const size_t n = ids.size();

  // Pre-compute the exact size we need so a single reserve() is enough.
  size_t total_size = 0;
  for (size_t i = 0; i < n; ++i) {
    total_size += 20 + 11 + 6 + messages[i].size() + 1; // upper bounds for ints + "|" + newline
  }

  std::string out;
  out.reserve(total_size);

  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    out.clear();

    char buffer[32];
    for (size_t i = 0; i < n; ++i) {
      // timestamp
      char* p = buffer;
      uint32_t t = timestamps[i];
      char* q = p + 20; // enough for 32-bit uint
      do {
        *--q = '0' + (t % 10);
        t /= 10;
      } while (t > 0);
      out.append(q, p + 20 - q);
      out.push_back('|');

      // id
      p = buffer;
      uint32_t id = ids[i];
      q = p + 20;
      do {
        *--q = '0' + (id % 10);
        id /= 10;
      } while (id > 0);
      out.append(q, p + 20 - q);
      out.push_back('|');

      // level
      p = buffer;
      uint16_t lvl = levels[i];
      q = p + 10;
      do {
        *--q = '0' + (lvl % 10);
        lvl /= 10;
      } while (lvl > 0);
      out.append(q, p + 10 - q);
      out.push_back('|');

      // message
      out.append(messages[i]);
      out.push_back('\n');
    }

    hash = checksum_text(out);
  }

  return hash;
}