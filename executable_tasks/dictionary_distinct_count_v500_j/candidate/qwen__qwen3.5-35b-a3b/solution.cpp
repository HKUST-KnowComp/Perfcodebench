#include "interface.h"

#include <string>
#include <unordered_set>
#include <vector>
#include <string_view>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  std::unordered_set<std::string_view> seen;
  for (const auto& code : codes) {
    seen.insert(std::string_view(dictionary[code]));
  }
  return static_cast<uint64_t>(seen.size());
}