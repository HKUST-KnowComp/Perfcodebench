#include "interface.h"

#include <string>
#include <vector>

namespace {

inline void fnv_update(uint64_t& h, unsigned char c) {
  h ^= static_cast<uint64_t>(c);
  h *= 1099511628211ULL;
}

inline void fnv_update_block(uint64_t& h, const char* data, std::size_t len) {
  for (std::size_t i = 0; i < len; ++i) {
    fnv_update(h, static_cast<unsigned char>(data[i]));
  }
}

template <typename T>
inline void hash_unsigned_dec(uint64_t& h, T v) {
  // Convert unsigned integer to decimal and stream into FNV-1a hash
  if (v == 0) {
    fnv_update(h, static_cast<unsigned char>('0'));
    return;
  }
  char buf[32];
  int idx = 0;
  while (v > 0) {
    T q = static_cast<T>(v / 10);
    unsigned digit = static_cast<unsigned>(v - q * 10);
    buf[idx++] = static_cast<char>('0' + digit);
    v = q;
  }
  while (idx--) {
    fnv_update(h, static_cast<unsigned char>(buf[idx]));
  }
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  if (iters <= 0) return 0ULL;

  uint64_t hash = 1469598103934665603ULL;  // FNV-1a offset basis

  const std::size_t n = ids.size();
  for (std::size_t i = 0; i < n; ++i) {
    hash_unsigned_dec<uint32_t>(hash, timestamps[i]);
    fnv_update(hash, static_cast<unsigned char>('|'));

    hash_unsigned_dec<uint32_t>(hash, ids[i]);
    fnv_update(hash, static_cast<unsigned char>('|'));

    hash_unsigned_dec<uint16_t>(hash, levels[i]);
    fnv_update(hash, static_cast<unsigned char>('|'));

    const std::string& msg = messages[i];
    if (!msg.empty()) {
      fnv_update_block(hash, msg.data(), msg.size());
    }
    fnv_update(hash, static_cast<unsigned char>('\n'));
  }

  return hash;
}
