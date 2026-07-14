#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  uint64_t count = 0;

  if (iters <= 0) return 0;
  if (codes.empty()) return 0;
  if (dictionary.empty()) return 0;

  const std::size_t dict_size = dictionary.size();

  // Fast path: if dictionary fits in uint16_t code space, count distinct codes
  // directly with a compact stamp table. Distinct decoded strings are counted,
  // so duplicate dictionary entries must be merged by string equality.
  if (dict_size <= 65536u) {
    std::vector<uint32_t> code_mark(dict_size, 0);
    std::vector<uint16_t> touched_codes;
    touched_codes.reserve(codes.size());

    std::vector<uint32_t> dict_group(dict_size, 0);
    uint32_t next_group = 1;

    for (int iter = 0; iter < iters; ++iter) {
      touched_codes.clear();
      uint32_t stamp = static_cast<uint32_t>(iter + 1);

      for (uint16_t code : codes) {
        const std::size_t idx = static_cast<std::size_t>(code);
        if (code_mark[idx] != stamp) {
          code_mark[idx] = stamp;
          touched_codes.push_back(code);
        }
      }

      // Assign equal dictionary strings the same group id among touched codes.
      for (uint16_t code : touched_codes) {
        const std::size_t idx = static_cast<std::size_t>(code);
        if (dict_group[idx] != 0) continue;
        dict_group[idx] = next_group;
        const std::string& s = dictionary[idx];
        for (std::size_t j = idx + 1; j < dict_size; ++j) {
          if (dict_group[j] == 0 && dictionary[j] == s) {
            dict_group[j] = next_group;
          }
        }
        ++next_group;
      }

      std::vector<uint32_t> group_seen(next_group, 0);
      uint64_t distinct = 0;
      for (uint16_t code : touched_codes) {
        const uint32_t g = dict_group[static_cast<std::size_t>(code)];
        if (group_seen[g] == 0) {
          group_seen[g] = 1;
          ++distinct;
        }
      }
      count = distinct;
    }
    return count;
  }

  // Fallback for oversized dictionary (should be rare under uint16_t codes).
  // Only indices addressable by codes can appear.
  const std::size_t addressable = 65536u;
  std::vector<uint32_t> code_mark(addressable, 0);
  std::vector<uint16_t> touched_codes;
  touched_codes.reserve(codes.size());

  for (int iter = 0; iter < iters; ++iter) {
    touched_codes.clear();
    const uint32_t stamp = static_cast<uint32_t>(iter + 1);
    for (uint16_t code : codes) {
      const std::size_t idx = static_cast<std::size_t>(code);
      if (code_mark[idx] != stamp) {
        code_mark[idx] = stamp;
        touched_codes.push_back(code);
      }
    }

    uint64_t distinct = 0;
    for (std::size_t i = 0; i < touched_codes.size(); ++i) {
      const std::string& s = dictionary[static_cast<std::size_t>(touched_codes[i])];
      bool found = false;
      for (std::size_t j = 0; j < i; ++j) {
        if (dictionary[static_cast<std::size_t>(touched_codes[j])] == s) {
          found = true;
          break;
        }
      }
      if (!found) ++distinct;
    }
    count = distinct;
  }

  return count;
}
