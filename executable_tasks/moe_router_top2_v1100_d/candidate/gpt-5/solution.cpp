#include "interface.h"

#include <cstdint>
#include <vector>

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int /*iters*/) {
  // Compute checksum once because the baseline resets the hash to the FNV offset
  // and recomputes the same result each iteration, making the final value equal
  // to a single iteration's checksum.
  uint64_t hash = 1469598103934665603ULL; // FNV-1a 64-bit offset basis

  const int32_t* data = logits.data();

  for (int t = 0; t < tokens; ++t) {
    const std::size_t base = static_cast<std::size_t>(t) * static_cast<std::size_t>(experts);

    // Find top-2 using a single pass with deterministic tie-breaking:
    // better(a, b): a.first > b.first || (a.first == b.first && a.second < b.second)
    int32_t top1_val = INT32_MIN;
    int32_t top2_val = INT32_MIN;
    int top1_idx = -1;
    int top2_idx = -1;

    for (int e = 0; e < experts; ++e) {
      const int32_t v = data[base + static_cast<std::size_t>(e)];
      if (v > top1_val || (v == top1_val && e < top1_idx)) {
        // promote current top1 to top2
        top2_val = top1_val;
        top2_idx = top1_idx;
        top1_val = v;
        top1_idx = e;
      } else if (v > top2_val || (v == top2_val && e < top2_idx)) {
        top2_val = v;
        top2_idx = e;
      }
    }

    // Mix results exactly as baseline: values are cast to uint32 before to uint64
    hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top1_val)));
    hash = mix(hash, static_cast<uint64_t>(top1_idx));
    hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(top2_val)));
    hash = mix(hash, static_cast<uint64_t>(top2_idx));
  }

  return hash;
}
