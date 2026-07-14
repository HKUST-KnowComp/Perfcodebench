#include "interface.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int /*iters*/) {
  const std::size_t dict_size = dictionary.size();
  if (dict_size == 0 || codes.empty()) return 0;

  // Deduplicate indices referenced by codes using a thread-local epoch table
  static thread_local std::vector<uint32_t> mark(65536, 0);
  static thread_local uint32_t epoch = 1;
  epoch++;
  if (epoch == 0) { // handle wrap-around extremely rarely
    std::fill(mark.begin(), mark.end(), 0);
    epoch = 1;
  }

  std::vector<uint16_t> unique_indices;
  unique_indices.reserve(std::min<std::size_t>(codes.size(), 65536));
  for (uint16_t code : codes) {
    // Safety check: ensure code is within dictionary bounds
    if (static_cast<std::size_t>(code) >= dict_size) continue;
    if (mark[code] != epoch) {
      mark[code] = epoch;
      unique_indices.push_back(code);
    }
  }

  // Count distinct decoded strings using string_view to avoid copies
  std::unordered_set<std::string_view> seen;
  seen.reserve(unique_indices.size() * 2);
  for (uint16_t idx : unique_indices) {
    const std::string& s = dictionary[idx];
    seen.insert(std::string_view(s.data(), s.size()));
  }

  return static_cast<uint64_t>(seen.size());
}
