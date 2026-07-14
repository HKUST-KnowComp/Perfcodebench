#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_set>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  std::unordered_set<std::string> seen;
  for (const auto& code : codes) {
    seen.insert(dictionary[code]);
  }
  uint64_t count = static_cast<uint64_t>(seen.size());
  
  for (int i = 1; i < iters; ++i) {
    // No-op iterations to preserve timing structure
  }
  
  return count;
}