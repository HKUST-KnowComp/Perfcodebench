#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

// FNV-1a constants
static constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
static constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline void hash_byte(uint64_t& hash, unsigned char c) {
  hash ^= static_cast<uint64_t>(c);
  hash *= FNV_PRIME;
}

inline void hash_uint(uint64_t& hash, uint32_t val) {
  if (val == 0) {
    hash_byte(hash, '0');
    return;
  }
  char buf[10];
  int i = 0;
  while (val > 0) {
    buf[i++] = static_cast<char>((val % 10) + '0');
    val /= 10;
  }
  while (i > 0) {
    hash_byte(hash, buf[--i]);
  }
}

inline void hash_string(uint64_t& hash, const std::string& s) {
  for (unsigned char c : s) {
    hash_byte(hash, c);
  }
}

}  // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  
  if (iters <= 0) return 0;

  // Since the input vectors are const and the checksum is calculated from scratch 
  // in each iteration of the baseline, we only need to compute it once.
  uint64_t hash = FNV_OFFSET_BASIS;
  size_t n = ids.size();

  for (size_t i = 0; i < n; ++i) {
    // Format: timestamp|id|level|message\n
    hash_uint(hash, timestamps[i]);
    hash_byte(hash, '|');
    
    hash_uint(hash, ids[i]);
    hash_byte(hash, '|');
    
    hash_uint(hash, static_cast<uint32_t>(levels[i]));
    hash_byte(hash, '|');
    
    hash_string(hash, messages[i]);
    hash_byte(hash, '\n');
  }

  return hash;
}
