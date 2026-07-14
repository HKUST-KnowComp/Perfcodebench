#include "interface.h"

#include <string>
#include <vector>

namespace {

static inline void hash_byte(uint64_t& h, unsigned char c) {
  h ^= static_cast<uint64_t>(c);
  h *= 1099511628211ULL; // FNV-1a prime
}

static inline void hash_char(uint64_t& h, char c) {
  hash_byte(h, static_cast<unsigned char>(c));
}

// Decimal formatting for unsigned integers (32-bit input)
static inline void hash_uint_dec(uint64_t& h, uint32_t v) {
  char buf[10]; // max digits for uint32_t
  int len = 0;
  do {
    uint32_t q = v / 10u;
    uint32_t r = v - q * 10u;
    buf[len++] = static_cast<char>('0' + r);
    v = q;
  } while (v != 0);
  // emit in forward order
  for (int i = len - 1; i >= 0; --i) {
    hash_byte(h, static_cast<unsigned char>(buf[i]));
  }
}

static inline void hash_str(uint64_t& h, const std::string& s) {
  const unsigned char* p = reinterpret_cast<const unsigned char*>(s.data());
  const unsigned char* e = p + s.size();
  while (p != e) {
    hash_byte(h, *p++);
  }
}

} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  const std::size_t n = ids.size();

  // Match baseline behavior when iters == 0
  if (iters <= 0) {
    return 0ULL;
  }

  uint64_t last_hash = 0ULL;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL; // FNV-1a offset basis

    const uint32_t* ts = timestamps.data();
    const uint32_t* idp = ids.data();
    const uint16_t* lvl = levels.data();
    const std::string* msg = messages.data();

    for (std::size_t i = 0; i < n; ++i) {
      // timestamp
      hash_uint_dec(h, ts[i]);
      hash_char(h, '|');
      // id
      hash_uint_dec(h, idp[i]);
      hash_char(h, '|');
      // level (uint16_t promoted)
      hash_uint_dec(h, static_cast<uint32_t>(lvl[i]));
      hash_char(h, '|');
      // message
      hash_str(h, msg[i]);
      // newline
      hash_char(h, '\n');
    }

    last_hash = h;
  }

  return last_hash;
}
