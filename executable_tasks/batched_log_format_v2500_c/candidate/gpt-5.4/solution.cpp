#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline std::size_t decimal_len_u32(uint32_t v) {
  std::size_t len = 1;
  while (v >= 10) {
    v /= 10;
    ++len;
  }
  return len;
}

inline std::size_t decimal_len_u16(uint16_t v) {
  std::size_t len = 1;
  while (v >= 10) {
    v /= 10;
    ++len;
  }
  return len;
}

inline void append_u32(std::string& out, uint32_t v) {
  char buf[10];
  int pos = 10;
  do {
    const uint32_t q = v / 10;
    buf[--pos] = static_cast<char>('0' + (v - q * 10));
    v = q;
  } while (v != 0);
  out.append(buf + pos, static_cast<std::size_t>(10 - pos));
}

inline void append_u16(std::string& out, uint16_t v) {
  char buf[5];
  int pos = 5;
  uint32_t x = v;
  do {
    const uint32_t q = x / 10;
    buf[--pos] = static_cast<char>('0' + (x - q * 10));
    x = q;
  } while (x != 0);
  out.append(buf + pos, static_cast<std::size_t>(5 - pos));
}

uint64_t checksum_text(const std::string& text) {
  uint64_t hash = 1469598103934665603ULL;
  const unsigned char* p = reinterpret_cast<const unsigned char*>(text.data());
  const unsigned char* end = p + text.size();
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
  std::string out;
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::size_t total_size = 0;
    for (std::size_t i = 0; i < n; ++i) {
      total_size += decimal_len_u32(timestamps[i]) + 1 +
                    decimal_len_u32(ids[i]) + 1 +
                    decimal_len_u16(levels[i]) + 1 +
                    messages[i].size() + 1;
    }

    out.clear();
    out.reserve(total_size);

    for (std::size_t i = 0; i < n; ++i) {
      append_u32(out, timestamps[i]);
      out.push_back('|');
      append_u32(out, ids[i]);
      out.push_back('|');
      append_u16(out, levels[i]);
      out.push_back('|');
      out += messages[i];
      out.push_back('\n');
    }

    hash = checksum_text(out);
  }

  return hash;
}
