#pragma once

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  if (iters <= 0) return 0;

  uint64_t sum = 0;
  std::vector<std::string> decoded;
  decoded.reserve(codes.size());

  for (std::size_t i = 0; i < codes.size(); ++i) {
    decoded.push_back(dictionary[static_cast<std::size_t>(codes[i])]);
  }

  for (std::size_t i = 0; i < decoded.size(); ++i) {
    const std::string& s = decoded[i];
    if (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm') {
      sum += static_cast<uint64_t>(weights[i]);
    }
  }

  return sum * static_cast<uint64_t>(iters);
}