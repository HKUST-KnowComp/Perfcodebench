#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

inline bool matches_prefix(const std::string& s) {
  if (s.size() < 2) return false;
  return s[0] == 'q' && s[1] <= 'm';
}

}  // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  // Pre-compute which dictionary entries match the prefix
  std::vector<bool> dict_matches(dictionary.size());
  for (std::size_t i = 0; i < dictionary.size(); ++i) {
    dict_matches[i] = matches_prefix(dictionary[i]);
  }
  
  uint64_t sum = 0;
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    for (std::size_t i = 0; i < codes.size(); ++i) {
      if (dict_matches[codes[i]]) {
        sum += static_cast<uint64_t>(weights[i]);
      }
    }
  }
  return sum;
}