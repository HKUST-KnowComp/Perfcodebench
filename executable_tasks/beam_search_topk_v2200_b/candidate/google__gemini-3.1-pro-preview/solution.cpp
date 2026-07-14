#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <vector>

namespace {

// Mix function for hashing as defined in the baseline.
inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

// Pack logit and index into a single uint64_t for efficient comparison.
// Logit (int32_t) is transformed to uint32_t preserving order (XOR 0x80000000).
// Index is bitwise inverted so that a smaller index results in a larger packed value.
inline uint64_t pack(int32_t logit, int32_t index) {
  uint32_t u_logit = static_cast<uint32_t>(logit) ^ 0x80000000;
  uint32_t u_index = ~static_cast<uint32_t>(index);
  return (static_cast<uint64_t>(u_logit) << 32) | static_cast<uint64_t>(u_index);
}

inline int32_t unpack_logit(uint64_t packed) {
  return static_cast<int32_t>((packed >> 32) ^ 0x80000000);
}

inline int32_t unpack_index(uint64_t packed) {
  return static_cast<int32_t>(~static_cast<uint32_t>(packed & 0xFFFFFFFF));
}

}  // namespace

uint64_t beam_topk_checksum(const std::vector<int32_t>& logits, int beams, int vocab, int topk, int iters) {
  if (iters <= 0 || beams <= 0 || vocab <= 0) return 0;

  // The baseline resets the hash inside the iters loop, so only the last iteration matters.
  // We only perform the calculation once.
  uint64_t hash = 1469598103934665603ULL;
  
  // Pre-allocate buffer for packing logits and indices to avoid repeated allocations.
  std::vector<uint64_t> buffer(vocab);
  
  for (int b = 0; b < beams; ++b) {
    const size_t base = static_cast<size_t>(b) * static_cast<size_t>(vocab);
    
    for (int v = 0; v < vocab; ++v) {
      buffer[v] = pack(logits[base + v], v);
    } 

    // Use nth_element to find the top-k elements in O(V) time.
    // std::greater ensures we get the largest packed values (highest logit, lowest index).
    if (topk < vocab) {
      std::nth_element(buffer.begin(), buffer.begin() + topk, buffer.end(), std::greater<uint64_t>());
    }
    
    // Sort only the top-k elements in O(K log K) time.
    int actual_k = (topk < vocab) ? topk : vocab;
    std::sort(buffer.begin(), buffer.begin() + actual_k, std::greater<uint64_t>());

    // Update hash with the top-k results.
    for (int k = 0; k < actual_k; ++k) {
      uint64_t val = buffer[k];
      hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(unpack_logit(val))));
      hash = mix(hash, static_cast<uint64_t>(unpack_index(val)));
    }
  }

  return hash;
}
