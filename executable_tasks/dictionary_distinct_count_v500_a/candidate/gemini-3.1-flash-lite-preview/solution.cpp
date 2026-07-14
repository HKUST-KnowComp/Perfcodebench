#include "interface.h"
#include <vector>
#include <cstring>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (codes.empty()) return 0;
  
  // The dictionary size is at most 65536 (uint16_t range).
  // We track which dictionary indices appear in the codes.
  std::vector<bool> seen(dictionary.size(), false);
  uint64_t distinct_count = 0;

  for (int iter = 0; iter < iters; ++iter) {
    std::fill(seen.begin(), seen.end(), false);
    distinct_count = 0;
    for (uint16_t code : codes) {
      if (!seen[code]) {
        seen[code] = true;
        distinct_count++;
      }
    }
  }
  
  // The problem asks for the count of distinct *decoded* strings.
  // If multiple dictionary indices map to the same string content,
  // we must group them. 
  std::vector<uint16_t> unique_indices;
  unique_indices.reserve(dictionary.size());
  
  // Identify which indices are actually used in the codes
  std::vector<bool> used_in_codes(dictionary.size(), false);
  for (uint16_t code : codes) used_in_codes[code] = true;

  // Group indices by string content
  // Using a simple approach: sort indices based on string content
  std::vector<size_t> p(dictionary.size());
  for(size_t i = 0; i < dictionary.size(); ++i) p[i] = i;
  
  std::sort(p.begin(), p.end(), [&](size_t a, size_t b) {
      return dictionary[a] < dictionary[b];
  });

  uint64_t final_count = 0;
  bool current_group_active = false;
  for (size_t i = 0; i < p.size(); ++i) {
      if (i > 0 && dictionary[p[i]] != dictionary[p[i-1]]) {
          if (current_group_active) final_count++;
          current_group_active = false;
      }
      if (used_in_codes[p[i]]) current_group_active = true;
  }
  if (current_group_active) final_count++;

  return final_count;
}