#include "interface.h"

#include <cstdint>
#include <vector>

namespace {
constexpr uint64_t FNV_OFFSET = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME  = 1099511628211ULL;
}

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens, const std::vector<int>& lengths, int batch, int max_len, int iters) {
  // Match baseline behavior: if iters <= 0, no hashing performed -> return 0.
  if (iters <= 0) return 0ULL;

  const uint32_t* tokens = padded_tokens.data();
  const int* lens = lengths.data();

  uint64_t hash = FNV_OFFSET;

  // Pack in the same order as baseline: for b in [0, batch), for i in [0, lengths[b))
  // Mix token first, then position (i as uint32_t), per element.
  for (int b = 0; b < batch; ++b) {
    int len = lens[b];
    const uint32_t* row = tokens + static_cast<size_t>(b) * static_cast<size_t>(max_len);
    for (int i = 0; i < len; ++i) {
      // Mix token
      hash ^= static_cast<uint64_t>(row[i]);
      hash *= FNV_PRIME;
      // Mix position (stored as uint32_t in baseline)
      hash ^= static_cast<uint64_t>(static_cast<uint32_t>(i));
      hash *= FNV_PRIME;
    }
  }

  return hash;
}
