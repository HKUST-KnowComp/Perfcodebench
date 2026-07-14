#include "interface.h"
#include <cstdint>
#include <string>
#include <vector>

namespace {
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline void update_uint(uint64_t& hash, uint32_t val) {
    char buf[12];
    int len = 0;
    if (val == 0) {
        buf[len++] = '0';
    } else {
        while (val > 0) {
            buf[len++] = '0' + (val % 10);
            val /= 10;
        }
    }
    for (int i = len - 1; i >= 0; --i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(buf[i]));
        hash *= FNV_PRIME;
    }
}

inline void update_char(uint64_t& hash, char c) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(c));
    hash *= FNV_PRIME;
}
} // namespace

uint64_t batched_log_checksum(
    const std::vector<uint32_t>& timestamps,
    const std::vector<uint32_t>& ids,
    const std::vector<uint16_t>& levels,
    const std::vector<std::string>& messages,
    int iters) {
  uint64_t hash = 0;
  const size_t n = ids.size();
  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET;
    for (size_t i = 0; i < n; ++i) {
      update_uint(hash, timestamps[i]);
      update_char(hash, '|');
      update_uint(hash, ids[i]);
      update_char(hash, '|');
      update_uint(hash, levels[i]);
      update_char(hash, '|');
      const std::string& msg = messages[i];
      const char* data = msg.data();
      const size_t mlen = msg.size();
      for (size_t k = 0; k < mlen; ++k) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[k]));
        hash *= FNV_PRIME;
      }
      update_char(hash, '\n');
    }
  }
  return hash;
}