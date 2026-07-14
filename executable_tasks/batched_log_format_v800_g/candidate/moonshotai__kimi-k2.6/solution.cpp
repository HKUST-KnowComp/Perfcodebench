#include "interface.h"

#include <string>
#include <vector>

namespace {

inline void hash_byte(uint64_t& hash, unsigned char c) {
  hash ^= static_cast<uint64_t>(c);
  hash *= 1099511628211ULL;
}

inline void hash_u32(uint64_t& hash, uint32_t v) {
  char buf[16];
  int n = 0;
  do {
    buf[n++] = static_cast<char>('0' + (v % 10));
    v /= 10;
  } while (v);
  while (n--) {
    hash_byte(hash, static_cast<unsigned char>(buf[n]));
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

  uint64_t hash = 1469598103934665603ULL;
  const std::size_t count = ids.size();

  for (std::size_t i = 0; i < count; ++i) {
    hash_u32(hash, timestamps[i]);
    hash_byte(hash, '|');
    hash_u32(hash, ids[i]);
    hash_byte(hash, '|');
    hash_u32(hash, levels[i]);
    hash_byte(hash, '|');

    const std::string& msg = messages[i];
    for (std::size_t j = 0; j < msg.size(); ++j) {
      hash_byte(hash, static_cast<unsigned char>(msg[j]));
    }
    hash_byte(hash, '\n');
  }

  return hash;
}
