#include "interface.h"
#include <vector>

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  const size_t n = left.size();
  const size_t m = right.size();
  uint64_t total_count = 0;
  for (int iter = 0; iter < iters; ++iter) {
    size_t i = 0, j = 0;
    uint32_t prev_val = 0;
    bool has_prev = false;
    uint64_t count = 0;
    while (i < n && j < m) {
      if (left[i] < right[j]) {
        uint32_t val = left[i];
        do { ++i; } while (i < n && left[i] == val);
        if (!has_prev || val != prev_val) {
          count++;
          prev_val = val;
          has_prev = true;
        }
      } else if (right[j] < left[i]) {
        uint32_t val = right[j];
        do { ++j; } while (j < m && right[j] == val);
        if (!has_prev || val != prev_val) {
          count++;
          prev_val = val;
          has_prev = true;
        }
      } else {
        uint32_t val = left[i];
        do { ++i; } while (i < n && left[i] == val);
        do { ++j; } while (j < m && right[j] == val);
        if (!has_prev || val != prev_val) {
          count++;
          prev_val = val;
          has_prev = true;
        }
      }
    }
    while (i < n) {
      uint32_t val = left[i];
      do { ++i; } while (i < n && left[i] == val);
      if (!has_prev || val != prev_val) {
        count++;
        prev_val = val;
        has_prev = true;
      }
    }
    while (j < m) {
      uint32_t val = right[j];
      do { ++j; } while (j < m && right[j] == val);
      if (!has_prev || val != prev_val) {
        count++;
        prev_val = val;
        has_prev = true;
      }
    }
    total_count = count;
  }
  return total_count;
}