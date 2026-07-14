#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

namespace {

bool matches_prefix(const std::string& s) {
  return s.size() >= 2 && s[0] == 'q' && s[1] <= 'm';
}

}  // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  // Precompute which dictionary entries match the prefix
  std::vector<bool> dict_matches(dictionary.size());
  for (std::size_t i = 0; i < dictionary.size(); ++i) {
    dict_matches[i] = matches_prefix(dictionary[i]);
  }

  uint64_t sum = 0;
  const std::size_t n = codes.size();
  const std::size_t n_unroll = n - (n % 4);

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t s0 = 0, s1 = 0, s2 = 0, s3 = 0;
    
    // Process 4 elements at a time
    for (std::size_t i = 0; i < n_unroll; i += 4) {
      s0 += dict_matches[codes[i]] ? static_cast<uint64_t>(weights[i]) : 0;
      s1 += dict_matches[codes[i + 1]] ? static_cast<uint64_t>(weights[i + 1]) : 0;
      s2 += dict_matches[codes[i + 2]] ? static_cast<uint64_t>(weights[i + 2]) : 0;
      s3 += dict_matches[codes[i + 3]] ? static_cast<uint64_t>(weights[i + 3]) : 0;
    }
    
    // Handle remainder
    for (std::size_t i = n_unroll; i < n; ++i) {
      s0 += dict_matches[codes[i]] ? static_cast<uint64_t>(weights[i]) : 0;
    }
    
    sum = s0 + s1 + s2 + s3;
  }
  
  return sum;
}