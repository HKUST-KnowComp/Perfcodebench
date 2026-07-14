#include "interface.h"

#include <string>
#include <vector>

namespace {

static inline void fnv1a_update(uint64_t& h, unsigned char c) {
  h ^= static_cast<uint64_t>(c);
  h *= 1099511628211ULL;
}

static inline void hash_char(uint64_t& h, char c) {
  fnv1a_update(h, static_cast<unsigned char>(c));
}

static inline void hash_bytes(uint64_t& h, const char* data, std::size_t len) {
  for (std::size_t i = 0; i < len; ++i) {
    fnv1a_update(h, static_cast<unsigned char>(data[i]));
  }
}

static inline void hash_uint32_dec(uint64_t& h, uint32_t v) {
  char buf[10];
  int idx = 0;
  do {
    uint32_t q = v / 10u;
    uint32_t r = v - q * 10u;
    buf[idx++] = static_cast<char>('0' + r);
    v = q;
  } while (v);
  for (int i = idx - 1; i >= 0; --i) {
    fnv1a_update(h, static_cast<unsigned char>(buf[i]));
  }
}

static inline void hash_uint16_dec(uint64_t& h, uint16_t v) {
  char buf[5];
  int idx = 0;
  uint32_t val = v; // promote to avoid repeated casts
  do {
    uint32_t q = val / 10u;
    uint32_t r = val - q * 10u;
    buf[idx++] = static_cast<char>('0' + r);
    val = q;
  } while (val);
  for (int i = idx - 1; i >= 0; --i) {
    fnv1a_update(h, static_cast<unsigned char>(buf[i]));
  }
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  if (iters <= 0) {
    return 0ULL;
  }

  uint64_t hash = 1469598103934665603ULL; // FNV-1a offset basis

  const std::size_t n = ids.size();
  for (std::size_t i = 0; i < n; ++i) {
    // timestamp
    hash_uint32_dec(hash, timestamps[i]);
    hash_char(hash, '|');
    // id
    hash_uint32_dec(hash, ids[i]);
    hash_char(hash, '|');
    // level
    hash_uint16_dec(hash, levels[i]);
    hash_char(hash, '|');
    // message
    const std::string& msg = messages[i];
    if (!msg.empty()) {
      hash_bytes(hash, msg.data(), msg.size());
    }
    // newline
    hash_char(hash, '\n');
  }

  return hash;
}
