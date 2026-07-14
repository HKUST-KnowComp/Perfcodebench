#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  std::unordered_map<std::string, uint16_t> str_to_id;
  std::vector<uint16_t> dict_ids(dictionary.size());
  uint16_t next_id = 0;

  for (std::size_t i = 0; i < dictionary.size(); ++i) {
    auto it = str_to_id.find(dictionary[i]);
    if (it == str_to_id.end()) {
      str_to_id[dictionary[i]] = next_id;
      dict_ids[i] = next_id++;
    } else {
      dict_ids[i] = it->second;
    }
  }

  std::vector<bool> seen_ids(next_id, false);
  uint64_t count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    count = 0;
    for (uint16_t code : codes) {
      uint16_t id = dict_ids[code];
      if (!seen_ids[id]) {
        seen_ids[id] = true;
        ++count;
      }
    }
    for (uint16_t i = 0; i < next_id; ++i) {
      seen_ids[i] = false;
    }
  }

  return count;
}