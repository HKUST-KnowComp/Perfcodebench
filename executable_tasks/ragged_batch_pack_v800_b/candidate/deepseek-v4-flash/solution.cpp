#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
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
  const uint64_t kFnvOffset = 1469598103934665603ULL;
  const uint32_t* tokens_ptr = padded_tokens.data();
  const int* lengths_ptr = lengths.data();
  const size_t max_len_s = static_cast<size_t>(max_len);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = kFnvOffset;
    for (int b = 0; b < batch; ++b) {
      const size_t base = static_cast<size_t>(b) * max_len_s;
      const int len = lengths_ptr[b];
      for (int i = 0; i < len; ++i) {
        const uint32_t token = tokens_ptr[base + static_cast<size_t>(i)];
        h = mix(h, token);
        h = mix(h, static_cast<uint32_t>(i));
      }
    }
    hash = h;
  }
  return hash;
}
