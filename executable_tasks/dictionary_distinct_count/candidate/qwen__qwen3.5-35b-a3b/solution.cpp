#include "interface.h"

#include <string>
#include <unordered_set>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  std::unordered_set<std::string> seen;
  seen.reserve(codes.size());
  for (uint16_t code : codes) {
    seen.insert(dictionary[code]);
  }
  return seen.size();
}