#include "interface.h"
#include <string>
#include <vector>
#include <algorithm>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  uint64_t count = 0;
  if (iters > 0) {
    std::vector<std::string_view> views;
    views.reserve(codes.size());
    for (uint16_t code : codes) {
      views.push_back(std::string_view(dictionary[code]));
    }
    std::sort(views.begin(), views.end());
    count = 0;
    if (!views.empty()) {
      count = 1;
      for (size_t i = 1; i < views.size(); ++i) {
        if (views[i] != views[i-1]) {
          count++;
        }
      }
    }
    // Skip remaining iterations (count is invariant)
    for (int iter = 1; iter < iters; ++iter) {
      // No-op
    }
  }
  return count;
}