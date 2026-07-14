#include "interface.h"

#include <cstdint>
#include <string>
#include <string_view>
#include <unordered_set>
#include <vector>
#include <algorithm>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (iters <= 0) return 0;
  if (codes.empty()) return 0;

  // Deduplicate code indices using a stamp array to avoid clearing 65536 entries each call.
  thread_local std::vector<uint32_t> stamps(65536, 0);
  thread_local uint32_t current_stamp = 1;
  uint32_t stamp = ++current_stamp;
  if (stamp == 0) {
    std::fill(stamps.begin(), stamps.end(), 0);
    stamp = ++current_stamp;
  }

  std::vector<uint16_t> unique_codes;
  unique_codes.reserve(codes.size());
  for (uint16_t c : codes) {
    uint32_t& slot = stamps[c];
    if (slot != stamp) {
      slot = stamp;
      unique_codes.push_back(c);
    }
  }

  std::unordered_set<std::string_view> seen;
  seen.reserve(unique_codes.size());
  for (uint16_t idx : unique_codes) {
    seen.emplace(dictionary[static_cast<std::size_t>(idx)]);
  }

  return static_cast<uint64_t>(seen.size());
}
