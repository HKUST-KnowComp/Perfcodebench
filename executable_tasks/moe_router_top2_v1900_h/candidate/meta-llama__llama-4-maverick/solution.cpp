#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

void top2(const int32_t* logits, int experts, uint64_t& hash) {
  int32_t max1 = logits[0];
  int max2 = std::numeric_limits<int32_t>::min();
  int argmax1 = 0;
  int argmax2 = 0;
  for (int e = 1; e < experts; ++e) {
    int32_t val = logits[e];
    if (val > max1) {
      max2 = max1;
      argmax2 = argmax1;
      max1 = val;
      argmax1 = e;
    } else if (val > max2 || (val == max1 && e < argmax1)) {
      max2 = val;
      argmax2 = e;
    }
  }
  hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(max1)));
  hash = mix(hash, static_cast<uint64_t>(argmax1));
  hash = mix(hash, static_cast<uint64_t>(static_cast<uint32_t>(max2)));
  hash = mix(hash, static_cast<uint64_t>(argmax2));
}

}  // namespace

uint64_t moe_router_checksum(const std::vector<int32_t>& logits, int tokens, int experts, int iters) {
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (int t = 0; t < tokens; ++t) {
      top2(logits.data() + static_cast<std::size_t>(t) * static_cast<std::size_t>(experts), experts, hash);
    }
  }
  return hash;
}