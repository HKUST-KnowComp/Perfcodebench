#include "interface.h"

#include <string>
#include <string_view>
#include <vector>
#include <algorithm>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  std::vector<std::string_view> views;
  views.reserve(codes.size());
  for (uint16_t code : codes) {
    views.push_back(dictionary[code]);
  }
  std::sort(views.begin(), views.end());
  auto last = std::unique(views.begin(), views.end());
  return static_cast<uint64_t>(std::distance(views.begin(), last));
}