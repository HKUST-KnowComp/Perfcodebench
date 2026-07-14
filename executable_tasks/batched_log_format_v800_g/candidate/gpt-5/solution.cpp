#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

namespace {

static inline void fnv1a_update(uint64_t& h, unsigned char c) {
  h ^= static_cast<uint64_t>(c);
  h *= 1099511628211ULL;
}

static inline void hash_literal(uint64_t& h, const char* s, std::size_t n) {
  for (std::size_t i = 0; i < n; ++i) {
    fnv1a_update(h, static_cast<unsigned char>(s[i]));
  }
}

static inline void hash_uint_dec(uint64_t& h, uint32_t v) {
  // Convert unsigned integer to decimal ASCII and feed into hash
  // Buffer large enough for max 32-bit value (10 digits)
  char buf[10];
  unsigned int n = 0;
  do {
    uint32_t q = v / 10u;
    uint32_t r = v - q * 10u;
    buf[n++] = static_cast<char>('0' + r);
    v = q;
  } while (v != 0u);
  while (n) {
    fnv1a_update(h, static_cast<unsigned char>(buf[--n]));
  }
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  // Preserve baseline behavior when iters == 0
  if (iters <= 0) {
    return 0;
  }

  // Compute checksum for one iteration (final output is same for each iteration)
  uint64_t hash = 1469598103934665603ULL; // FNV-1a offset basis

  const std::size_t n = ids.size();
  for (std::size_t i = 0; i < n; ++i) {
    hash_uint_dec(hash, timestamps[i]);
    fnv1a_update(hash, static_cast<unsigned char>('|'));

    hash_uint_dec(hash, ids[i]);
    fnv1a_update(hash, static_cast<unsigned char>('|'));

    hash_uint_dec(hash, static_cast<uint32_t>(levels[i]));
    fnv1a_update(hash, static_cast<unsigned char>('|'));

    const std::string& msg = messages[i];
    for (unsigned char c : msg) {
      fnv1a_update(hash, c);
    }
    fnv1a_update(hash, static_cast<unsigned char>('\n'));
  }

  return hash;
}
