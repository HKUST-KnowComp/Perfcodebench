#include "interface.h"

#include <string>
#include <unordered_map>
#include <vector>
#include <algorithm>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (dictionary.empty() || codes.empty() || iters <= 0) {
    return 0;
  }

  // Preprocess dictionary to map each code to a unique integer ID for its string
  std::unordered_map<std::string, uint32_t> str_to_uid;
  std::vector<uint32_t> code_to_uid(dictionary.size());
  uint32_t next_uid = 0;
  for (size_t i = 0; i < dictionary.size(); ++i) {
    const auto& s = dictionary[i];
    auto it = str_to_uid.find(s);
    if (it == str_to_uid.end()) {
      str_to_uid[s] = next_uid;
      code_to_uid[i] = next_uid;
      next_uid++;
    } else {
      code_to_uid[i] = it->second;
    }
  }

  const uint32_t total_uids = next_uid;
  std::vector<bool> seen(total_uids, false);
  uint64_t count = 0;

  // Run iteration loop with low-overhead integer operations only
  for (int iter = 0; iter < iters; ++iter) {
    std::fill(seen.begin(), seen.end(), false);
    uint64_t temp_count = 0;
    for (uint16_t code : codes) {
      const uint32_t uid = code_to_uid[static_cast<size_t>(code)];
      if (!seen[uid]) {
        seen[uid] = true;
        temp_count++;
      }
    }
    count = temp_count;
  }
  return count;
}