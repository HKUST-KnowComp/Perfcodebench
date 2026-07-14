#include "interface.h"
#include <vector>
#include <unordered_set>
#include <algorithm>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (codes.empty()) return 0;

  // Pre-calculate the unique dictionary strings to handle cases where 
  // multiple dictionary indices might point to the same string content.
  // We map each unique string to a unique ID.
  std::vector<uint32_t> code_to_id(dictionary.size());
  
  // To handle the 'distinct decoded strings' requirement correctly, 
  // we must group dictionary indices that represent the same string.
  struct DictEntry {
    const std::string* str;
    size_t original_idx;
  };
  
  std::vector<size_t> dict_indices(dictionary.size());
  for (size_t i = 0; i < dictionary.size(); ++i) dict_indices[i] = i;

  std::sort(dict_indices.begin(), dict_indices.end(), [&](size_t a, size_t b) {
    return dictionary[a] < dictionary[b];
  });

  uint32_t current_id = 0;
  if (!dict_indices.empty()) {
    code_to_id[dict_indices[0]] = 0;
    for (size_t i = 1; i < dict_indices.size(); ++i) {
      if (dictionary[dict_indices[i]] != dictionary[dict_indices[i - 1]]) {
        current_id++;
      }
      code_to_id[dict_indices[i]] = current_id;
    }
  }

  // Convert the input codes into their corresponding unique string IDs
  std::vector<uint32_t> mapped_codes(codes.size());
  for (size_t i = 0; i < codes.size(); ++i) {
    mapped_codes[i] = code_to_id[static_cast<size_t>(codes[i])];
  }

  uint64_t final_count = 0;
  // Reusable buffer for sorting/uniquifying to avoid allocations in the loop
  std::vector<uint32_t> working_buffer = mapped_codes;

  for (int iter = 0; iter < iters; ++iter) {
    // The problem asks for the count of distinct decoded strings.
    // Since we mapped identical strings to the same ID, we just count unique IDs.
    // We use sort + unique on the integer IDs for maximum performance.
    std::sort(working_buffer.begin(), working_buffer.end());
    auto it = std::unique(working_buffer.begin(), working_buffer.end());
    final_count = static_cast<uint64_t>(std::distance(working_buffer.begin(), it));
  }

  return final_count;
}