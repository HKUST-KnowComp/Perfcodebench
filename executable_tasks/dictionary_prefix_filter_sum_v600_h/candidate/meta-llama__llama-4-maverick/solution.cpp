#include "interface.h"

#include <string>
#include <vector>

namespace {

bool matches_prefix(const std::string& s) {
  // Using a single comparison to check the first two characters
  return s.size() >= 2 && (s[0] == 'q' && s[1] <= 'm');
}

}  // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  uint64_t sum = 0;
  // Precompute the decoded strings and store them in a separate vector to avoid repeated decoding
  std::vector<std::string> decoded(codes.size());
  for (std::size_t i = 0; i < codes.size(); ++i) {
    decoded[i] = dictionary[static_cast<std::size_t>(codes[i])];
  }
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t iter_sum = 0;
    for (std::size_t i = 0; i < decoded.size(); ++i) {
      if (matches_prefix(decoded[i])) {
        iter_sum += static_cast<uint64_t>(weights[i]);
      }
    }
    sum = iter_sum; // Only the last iteration's sum is returned, so we can avoid updating sum in each iteration
  }
  return sum;
}
