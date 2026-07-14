#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (codes.empty()) return 0;

  // maximum code value (all codes are valid indices into dictionary)
  uint16_t max_code = *std::max_element(codes.begin(), codes.end());

  // map each possible code to a unique ID for the string it represents
  std::vector<int> code_to_id(max_code + 1, -1);
  std::unordered_map<std::string, int> string_to_id;
  int next_id = 0;
  for (uint16_t i = 0; i <= max_code; ++i) {
    const std::string& s = dictionary[i];
    auto it = string_to_id.find(s);
    if (it == string_to_id.end()) {
      string_to_id[s] = next_id;
      code_to_id[i] = next_id;
      ++next_id;
    } else {
      code_to_id[i] = it->second;
    }
  }
  int num_distinct = next_id;

  // generation counter array – avoids resetting last_seen each iteration
  std::vector<int> last_seen(num_distinct, 0);
  int cur_iter = 1;
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t cur_count = 0;
    for (uint16_t c : codes) {
      int id = code_to_id[c];
      if (last_seen[id] != cur_iter) {
        last_seen[id] = cur_iter;
        ++cur_count;
      }
    }
    count = cur_count;
    ++cur_iter;
  }
  return count;
}