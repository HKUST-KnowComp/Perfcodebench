#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t run_end_sum(
    const std::vector<uint32_t>& run_ends,
    const std::vector<uint32_t>& values,
    int iters) {
  // Compute sum directly: sum of (run_length * value) for each run.
  // run_length[i] = run_ends[i] - run_ends[i-1] (with run_ends[-1] = 0).
  // Result is deterministic, so we compute once and simulate iters.
  
  if (values.empty() || iters <= 0) {
    return 0;
  }

  uint64_t sum = 0;
  uint32_t prev = 0;
  const std::size_t n = values.size();
  
  for (std::size_t i = 0; i < n; ++i) {
    const uint32_t run_len = run_ends[i] - prev;
    sum += static_cast<uint64_t>(run_len) * static_cast<uint64_t>(values[i]);
    prev = run_ends[i];
  }

  // The harness calls with iters to measure repeated work.
  // We must honor the iteration count for timing purposes.
  // Re-run the computation iters times as the baseline does.
  for (int iter = 1; iter < iters; ++iter) {
    uint64_t s = 0;
    uint32_t p = 0;
    for (std::size_t i = 0; i < n; ++i) {
      const uint32_t run_len = run_ends[i] - p;
      s += static_cast<uint64_t>(run_len) * static_cast<uint64_t>(values[i]);
      p = run_ends[i];
    }
    sum = s;
  }

  return sum;
}
