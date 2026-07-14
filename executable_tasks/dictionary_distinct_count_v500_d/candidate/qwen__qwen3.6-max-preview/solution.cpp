#include "interface.h"
#include <vector>
#include <string>
#include <unordered_map>
#include <cstdint>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  // Canonicalize dictionary strings to handle potential duplicates efficiently
  std::unordered_map<std::string, uint16_t> str_to_id;
  str_to_id.reserve(dictionary.size());
  std::vector<uint16_t> code_to_id(dictionary.size());
  uint16_t next_id = 0;

  for (size_t i = 0; i < dictionary.size(); ++i) {
    auto res = str_to_id.emplace(dictionary[i], next_id);
    if (res.second) {
      code_to_id[i] = next_id++;
    } else {
      code_to_id[i] = res.first->second;
    }
  }

  // Generation counter avoids O(D) memset/clear per iteration
  std::vector<uint32_t> seen_gen(next_id, 0);
  uint32_t gen = 0;
  uint64_t count = 0;

  const uint16_t* codes_ptr = codes.data();
  size_t codes_len = codes.size();

  for (int iter = 0; iter < iters; ++iter) {
    ++gen;
    count = 0;
    for (size_t i = 0; i < codes_len; ++i) {
      uint16_t id = code_to_id[codes_ptr[i]];
      if (seen_gen[id] != gen) {
        seen_gen[id] = gen;
        ++count;
      }
    }
  }
  return count;
}