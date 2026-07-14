#pragma once

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  if (codes.empty()) return 0;

  std::vector<bool> matches;
  matches.reserve(codes.size());

  for (const auto& code : codes) {
    const std::string& s = dictionary[code];
    bool match = false;
    if (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm') {
      match = true;
    }
    matches.push_back(match);
  }

  uint64_t sum = 0;
  for (std::size_t i = 0; i < weights.size(); ++i) {
    if (matches[i]) {
      sum += static_cast<uint64_t>(weights[i]);
    }
  }

  return sum;
}