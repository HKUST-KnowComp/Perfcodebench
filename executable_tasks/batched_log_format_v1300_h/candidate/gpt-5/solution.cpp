#include "interface.h"

namespace {

static inline void fnv1a_update_char(uint64_t& hash, unsigned char c) {
  hash ^= static_cast<uint64_t>(c);
  hash *= 1099511628211ULL;
}

static inline void fnv1a_update(uint64_t& hash, const char* data, std::size_t len) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(data);
  for (std::size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(p[i]);
    hash *= 1099511628211ULL;
  }
}

static inline void fnv1a_update_uint_dec(uint64_t& hash, uint32_t v) {
  if (v == 0) {
    fnv1a_update_char(hash, static_cast<unsigned char>('0'));
    return;
  }
  char buf[10];
  int n = 0;
  while (v != 0) {
    uint32_t q = v / 10;
    uint32_t r = v - q * 10; // faster than modulo on many platforms
    buf[n++] = static_cast<char>('0' + r);
    v = q;
  }
  for (int i = n - 1; i >= 0; --i) {
    fnv1a_update_char(hash, static_cast<unsigned char>(buf[i]));
  }
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL; // FNV-1a offset basis
    const std::size_t n = ids.size();
    for (std::size_t i = 0; i < n; ++i) {
      // timestamp
      fnv1a_update_uint_dec(h, timestamps[i]);
      fnv1a_update_char(h, static_cast<unsigned char>('|'));
      // id
      fnv1a_update_uint_dec(h, ids[i]);
      fnv1a_update_char(h, static_cast<unsigned char>('|'));
      // level
      fnv1a_update_uint_dec(h, static_cast<uint32_t>(levels[i]));
      fnv1a_update_char(h, static_cast<unsigned char>('|'));
      // message
      const std::string& msg = messages[i];
      fnv1a_update(h, msg.data(), msg.size());
      // newline
      fnv1a_update_char(h, static_cast<unsigned char>('\n'));
    }
    hash = h;
  }
  return hash;
}
