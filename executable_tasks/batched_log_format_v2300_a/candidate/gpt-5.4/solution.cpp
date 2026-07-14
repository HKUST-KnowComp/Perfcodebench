#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

static inline uint64_t fnv1a_append_byte(uint64_t hash, unsigned char c) {
  hash ^= static_cast<uint64_t>(c);
  hash *= 1099511628211ULL;
  return hash;
}

static inline uint64_t append_u32_to_hash(uint64_t hash, uint32_t v) {
  char buf[10];
  int pos = 10;
  do {
    uint32_t q = v / 10;
    uint32_t r = v - q * 10;
    buf[--pos] = static_cast<char>('0' + r);
    v = q;
  } while (v != 0);
  for (int i = pos; i < 10; ++i) {
    hash = fnv1a_append_byte(hash, static_cast<unsigned char>(buf[i]));
  }
  return hash;
}

static inline uint64_t append_u16_to_hash(uint64_t hash, uint16_t v) {
  char buf[5];
  int pos = 5;
  do {
    uint16_t q = static_cast<uint16_t>(v / 10);
    uint16_t r = static_cast<uint16_t>(v - q * 10);
    buf[--pos] = static_cast<char>('0' + r);
    v = q;
  } while (v != 0);
  for (int i = pos; i < 5; ++i) {
    hash = fnv1a_append_byte(hash, static_cast<unsigned char>(buf[i]));
  }
  return hash;
}

static inline uint64_t append_string_to_hash(uint64_t hash, const std::string& s) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(s.data());
  const std::size_t n = s.size();
  for (std::size_t i = 0; i < n; ++i) {
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
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    for (std::size_t i = 0; i < n; ++i) {
      h = append_u32_to_hash(h, timestamps[i]);
      h = fnv1a_append_byte(h, static_cast<unsigned char>('|'));
      h = append_u32_to_hash(h, ids[i]);
      h = fnv1a_append_byte(h, static_cast<unsigned char>('|'));
      h = append_u16_to_hash(h, levels[i]);
      h = fnv1a_append_byte(h, static_cast<unsigned char>('|'));
      h = append_string_to_hash(h, messages[i]);
      h = fnv1a_append_byte(h, static_cast<unsigned char>('\n'));
    }
    hash = h;
  }

  return hash;
}
