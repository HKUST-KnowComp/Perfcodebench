#include "interface.h"

#include <string>
#include <unordered_set>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  std::vector<std::string> decoded;
  std::unordered_set<std::string> seen;
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    decoded.resize(codes.size());
    for (std::size_t i = 0; i < codes.size(); ++i) {
      decoded[i] = dictionary[static_cast<std::size_t>(codes[i])];
    }
    seen.clear();
    for (const std::string& s : decoded) {
      seen.insert(s);
    }
    count = static_cast<uint64_t>(seen.size());
  }
  return count;
}
