#include "interface.h"

uint64_t sorted_union_count(
    const std::vector<uint32_t>& left,
    const std::vector<uint32_t>& right,
    int iters) {
  if (iters <= 0) return 0;
  const size_t n = left.size();
  const size_t m = right.size();
  size_t i = 0, j = 0;
  uint64_t count = 0;
  uint32_t last = 0;
  bool has_last = false;
  while (i < n && j < m) {
    uint32_t val;
    if (left[i] < right[j]) {
      val = left[i++];
    } else if (right[j] < left[i]) {
      val = right[j++];
    } else {
      val = left[i];
      ++i; ++j;
    }
    if (!has_last || val != last) {
      ++count;
      last = val;
      has_last = true;
    }
  }
  while (i < n) {
    uint32_t val = left[i++];
    if (!has_last || val != last) {
      ++count;
      last = val;
      has_last = true;
    }
  }
  while (j < m) {
    uint32_t val = right[j++];
    if (!has_last || val != last) {
      ++count;
      last = val;
      has_last = true;
    }
  }
  return count;
}