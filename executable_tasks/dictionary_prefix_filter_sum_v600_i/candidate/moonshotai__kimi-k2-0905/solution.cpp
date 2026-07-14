#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

bool matches_prefix(const std::string& s) {
  return s[0] == 'q' && s[1] <= 'm';
}

}  // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  const size_t n = codes.size();
  std::vector<uint64_t> pref_weight(n);

  // one-time preprocessing: compute matching weight per row
  for (size_t i = 0; i < n; ++i) {
    const std::string& s = dictionary[static_cast<size_t>(codes[i])];
    pref_weight[i] = (matches_prefix(s) ? static_cast<uint64_t>(weights[i]) : 0);
  }

  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // vectorizable, branch-free summation
    uint64_t local = 0;
    size_t i = 0;
    // manual 4× unroll for better ILP / SIMD
    for (; i + 4 <= n; i += 4) {
      local += pref_weight[i]   + pref_weight[i+1] +
               pref_weight[i+2] + pref_weight[i+3];
    }
    for (; i < n; ++i) {
      local += pref_weight[i];
    }
    sum = local;
  }
  return sum;
}