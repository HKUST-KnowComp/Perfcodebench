#include "interface.h"

#include <vector>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens,
                              const std::vector<int>& lengths,
                              int batch,
                              int max_len,
                              int iters) {
  // Pre-compute the total number of actual tokens once.
  int total = 0;
  for (int b = 0; b < batch; ++b) total += lengths[static_cast<size_t>(b)];

  // Re-usable buffer; reserve once.
  std::vector<uint32_t> packed;
  packed.reserve(2 * static_cast<size_t>(total));   // tokens and positions interleaved

  uint64_t hash = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    packed.clear();
    hash = 1469598103934665603ULL;

    const uint32_t* base = padded_tokens.data();
    const int* lens = lengths.data();

    for (int b = 0; b < batch; ++b) {
      int len = lens[b];
      const uint32_t* row = base + b * max_len;

      for (int i = 0; i < len; ++i) {
        uint32_t token = row[i];
        uint32_t pos   = static_cast<uint32_t>(i);

        packed.push_back(token);
        packed.push_back(pos);

        hash = mix(hash, token);
        hash = mix(hash, pos);
      }
    }
  }
  return hash;
}