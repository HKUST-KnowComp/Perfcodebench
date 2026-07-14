#include "interface.h"

#include <string_view>
#include <unordered_set>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (iters <= 0) {
    return 0;
  }
  std::unordered_set<std::string_view> seen;
  seen.reserve(codes.size());
  for (std::size_t i = 0; i < codes.size(); ++i) {
    seen.emplace(dictionary[static_cast<std::size_t>(codes[i])]);
  }
  return static_cast<uint64_t>(seen.size());
}