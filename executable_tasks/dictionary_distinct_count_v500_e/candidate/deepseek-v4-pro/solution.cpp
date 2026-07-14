#include "interface.h"

#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  (void)iters; // irrelevant; just compute once
  std::unordered_set<std::string_view> distinct;
  for (std::size_t i = 0; i < codes.size(); ++i) {
    distinct.insert(dictionary[static_cast<std::size_t>(codes[i])]);
  }
  return static_cast<uint64_t>(distinct.size());
}
