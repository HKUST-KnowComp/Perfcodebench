#include "interface.h"
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  uint64_t count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    uint32_t left_idx = 0;
    uint32_t right_idx = 0;
    std::vector<uint32_t> unique_values;
    while (left_idx < left.size() && right_idx < right.size()) {
      if (left[left_idx] < right[right_idx]) {
        if (unique_values.empty() || unique_values.back() != left[left_idx]) {
          unique_values.push_back(left[left_idx]);
        }
        ++left_idx;
      } else if (left[left_idx] > right[right_idx]) {
        if (unique_values.empty() || unique_values.back() != right[right_idx]) {
          unique_values.push_back(right[right_idx]);
        }
        ++right_idx;
      } else {
        if (unique_values.empty() || unique_values.back() != left[left_idx]) {
          unique_values.push_back(left[left_idx]);
        }
        ++left_idx;
        ++right_idx;
      }
    }
    while (left_idx < left.size()) {
      if (unique_values.empty() || unique_values.back() != left[left_idx]) {
        unique_values.push_back(left[left_idx]);
      }
      ++left_idx;
    }
    while (right_idx < right.size()) {
      if (unique_values.empty() || unique_values.back() != right[right_idx]) {
        unique_values.push_back(right[right_idx]);
      }
      ++right_idx;
    }
    count = unique_values.size();
  }
  return count;
}