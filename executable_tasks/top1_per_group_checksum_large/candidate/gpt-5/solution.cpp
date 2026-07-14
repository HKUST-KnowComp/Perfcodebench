#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

static inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;  // FNV-1a prime
  return hash;
}

}  // namespace

uint64_t top1_group_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count,
    int iters) {
  const std::size_t N = groups.size();

  // Dense storage for per-group maxima and per-iteration stamps.
  std::vector<uint32_t> max_vals(group_count);
  std::vector<uint32_t> stamp(group_count, 0u);

  uint32_t gen = 0u;  // generation counter
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // Advance generation; handle wraparound by resetting stamps if it occurs (extremely unlikely).
    ++gen;
    if (gen == 0u) {
      // Wrapped around; reset stamps to 0 and use gen=1.
      std::fill(stamp.begin(), stamp.end(), 0u);
      gen = 1u;
    }

    // Compute per-group maxima for this iteration using generation stamps.
    const uint32_t* gptr = groups.data();
    const uint32_t* vptr = values.data();
    for (std::size_t i = 0; i < N; ++i) {
      const uint32_t g = gptr[i];
      const uint32_t v = vptr[i];
      if (stamp[g] != gen) {
        stamp[g] = gen;
        max_vals[g] = v;
      } else if (v > max_vals[g]) {
        max_vals[g] = v;
      }
    }

    // Hash over all groups (dense range [0, group_count)).
    hash = 1469598103934665603ULL;  // FNV-1a offset basis
    const uint32_t* mv = max_vals.data();
    const uint32_t* st = stamp.data();
    for (uint32_t g = 0; g < group_count; ++g) {
      // Branchless select: include value only if seen in this generation, else 0.
      uint64_t m = static_cast<uint64_t>(-(st[g] == gen));
      uint64_t val = static_cast<uint64_t>(mv[g]) & m;
      hash = mix(hash, val);
    }
  }

  return hash;
}
