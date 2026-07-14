#include "interface.h"

#include <cstdint>
#include <string>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  if (iters <= 0) return 0;

  const std::size_t dict_size = dictionary.size();
  if (dict_size == 0 || codes.empty()) return 0;

  // Count distinct decoded strings in the dictionary itself.
  // Multiple dictionary entries may decode to equal strings, so we must
  // deduplicate by string content, not by code value.
  std::vector<uint32_t> order(dict_size);
  for (std::size_t i = 0; i < dict_size; ++i) {
    order[i] = static_cast<uint32_t>(i);
  }

  struct LessByString {
    const std::vector<std::string>* dict;
    bool operator()(uint32_t a, uint32_t b) const {
      return (*dict)[a] < (*dict)[b];
    }
  } cmp{&dictionary};

  // Iterative bottom-up mergesort on indices to avoid extra dependencies.
  std::vector<uint32_t> tmp(dict_size);
  for (std::size_t width = 1; width < dict_size; width <<= 1) {
    for (std::size_t left = 0; left < dict_size; left += (width << 1)) {
      const std::size_t mid = (left + width < dict_size) ? (left + width) : dict_size;
      const std::size_t right = (mid + width < dict_size) ? (mid + width) : dict_size;

      std::size_t i = left;
      std::size_t j = mid;
      std::size_t k = left;
      while (i < mid && j < right) {
        if (cmp(order[j], order[i])) {
          tmp[k++] = order[j++];
        } else {
          tmp[k++] = order[i++];
        }
      }
      while (i < mid) tmp[k++] = order[i++];
      while (j < right) tmp[k++] = order[j++];
    }
    for (std::size_t i = 0; i < dict_size; ++i) order[i] = tmp[i];
  }

  std::vector<uint32_t> class_of_code(dict_size);
  uint32_t class_count = 0;
  {
    uint32_t current_class = 0;
    class_of_code[order[0]] = current_class;
    for (std::size_t i = 1; i < dict_size; ++i) {
      if (dictionary[order[i - 1]] != dictionary[order[i]]) {
        ++current_class;
      }
      class_of_code[order[i]] = current_class;
    }
    class_count = current_class + 1;
  }

  // Mark which distinct decoded strings appear in codes.
  std::vector<uint8_t> seen(class_count, 0);
  uint64_t distinct = 0;
  for (std::size_t i = 0, n = codes.size(); i < n; ++i) {
    const std::size_t code = static_cast<std::size_t>(codes[i]);
    const uint32_t cls = class_of_code[code];
    if (!seen[cls]) {
      seen[cls] = 1;
      ++distinct;
      if (distinct == class_count) break;
    }
  }

  // Result is identical for every iteration.
  return distinct;
}
