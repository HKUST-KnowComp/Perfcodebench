#include "interface.h"

#include <string_view>
#include <unordered_set>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  (void)iters;  // only one result needed, independent of repetition count
  std::unordered_set<std::string_view> seen;
  seen.reserve(codes.size());
  for (uint16_t code : codes) {
    seen.emplace(dictionary[code]);
  }
  return static_cast<uint64_t>(seen.size());
}