#include "interface.h"

namespace {
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME  = 1099511628211ULL;
}

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  if (iters <= 0) return 0ULL;

  const uint32_t* tokens = padded_tokens.data();
  const int* lens = lengths.data();

  uint64_t hash = FNV_OFFSET;
  for (int b = 0; b < batch; ++b) {
    const uint32_t* row = tokens + static_cast<std::size_t>(b) * static_cast<std::size_t>(max_len);
    int len = lens[static_cast<std::size_t>(b)];
    for (int i = 0; i < len; ++i) {
      hash ^= static_cast<uint64_t>(row[i]);
      hash *= FNV_PRIME;
      hash ^= static_cast<uint64_t>(static_cast<uint32_t>(i));
      hash *= FNV_PRIME;
    }
  }
  return hash;
}
