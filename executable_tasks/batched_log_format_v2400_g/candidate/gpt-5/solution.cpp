#include "interface.h"

namespace {

static inline void fnv1a_update_byte(uint64_t& h, unsigned char c) {
  h ^= static_cast<uint64_t>(c);
  h *= 1099511628211ULL;
}

static inline void fnv1a_update(uint64_t& h, const char* data, std::size_t len) {
  for (std::size_t i = 0; i < len; ++i) {
    fnv1a_update_byte(h, static_cast<unsigned char>(data[i]));
  }
}

// Convert unsigned 32-bit integer to decimal and feed into hash
static inline void append_u32(uint64_t& h, uint32_t v) {
  char buf[10];
  int len = 0;
  if (v == 0) {
    buf[len++] = '0';
  } else {
    char tmp[10];
    int tlen = 0;
    while (v > 0) {
      uint32_t q = v / 10;
      uint32_t digit = v - q * 10;
      tmp[tlen++] = static_cast<char>('0' + digit);
      v = q;
    }
    // reverse into buf
    while (tlen--) {
      buf[len++] = tmp[tlen];
    }
  }
  fnv1a_update(h, buf, static_cast<std::size_t>(len));
}

static inline void append_char(uint64_t& h, char c) {
  fnv1a_update_byte(h, static_cast<unsigned char>(c));
}

static inline void append_str(uint64_t& h, const std::string& s) {
  if (!s.empty()) {
    fnv1a_update(h, s.data(), s.size());
  }
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  if (iters <= 0) {
    return 0;
  }

  uint64_t hash = 1469598103934665603ULL; // FNV-1a offset basis

  const std::size_t n = ids.size();
  for (std::size_t i = 0; i < n; ++i) {
    append_u32(hash, timestamps[i]);
    append_char(hash, '|');
    append_u32(hash, ids[i]);
    append_char(hash, '|');
    append_u32(hash, static_cast<uint32_t>(levels[i]));
    append_char(hash, '|');
    append_str(hash, messages[i]);
    append_char(hash, '\n');
  }

  return hash;
}
