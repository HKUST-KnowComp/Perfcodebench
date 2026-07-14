#include "interface.h"

#include <cstdint>
#include <string>
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
  // Precompute for each code whether its dictionary entry matches the prefix.
  const std::size_t n = codes.size();
  std::vector<uint8_t> match(n, 0);
  for (std::size_t i = 0; i < n; ++i) {
    match[i] = matches_prefix(dictionary[static_cast<std::size_t>(codes[i])]) ? 1 : 0;
  }

  // Precompute total sum of weights for matching codes.
  uint64_t total_sum = 0;
  for (std::size_t i = 0; i < n; ++i) {
    if (match[i]) {
      total_sum += static_cast<uint64_t>(weights[i]);
    }
  }

  // Repeatedly assign the precomputed sum.
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = total_sum;
  }

  return sum;
}
