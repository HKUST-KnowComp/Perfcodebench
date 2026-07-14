#include "interface.h"

#include <string>
#include <vector>

namespace {

constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline void hash_byte(uint64_t& hash, unsigned char byte) {
  hash ^= static_cast<uint64_t>(byte);
  hash *= FNV_PRIME;
}

inline void hash_u32(uint64_t& hash, uint32_t value) {
  char digits[16];
  int count = 0;
  do {
    digits[count++] = static_cast<char>('0' + (value % 10));
    value /= 10;
  } while (value != 0);
  for (int i = count - 1; i >= 0; --i) {
    hash_byte(hash, static_cast<unsigned char>(digits[i]));
  }
}

inline void hash_u16(uint64_t& hash, uint16_t value) {
  char digits[8];
  int count = 0;
  do {
    digits[count++] = static_cast<char>('0' + (value % 10));
    value /= 10;
  } while (value != 0);
  for (int i = count - 1; i >= 0; --i) {
    hash_byte(hash, static_cast<unsigned char>(digits[i]));
  }
}

inline void hash_string(uint64_t& hash, const std::string& str) {
  const char* data = str.data();
  const std::size_t len = str.size();
  for (std::size_t i = 0; i < len; ++i) {
    hash_byte(hash, static_cast<unsigned char>(data[i]));
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

  uint64_t hash = FNV_OFFSET_BASIS;
  const std::size_t n = ids.size();

  for (std::size_t i = 0; i < n; ++i) {
    hash_u32(hash, timestamps[i]);
    hash_byte(hash, '|');
    hash_u32(hash, ids[i]);
    hash_byte(hash, '|');
    hash_u16(hash, levels[i]);
    hash_byte(hash, '|');
    hash_string(hash, messages[i]);
    hash_byte(hash, '\n');
  }

  return hash;
}
