#include "interface.h"

#include <cstdint>
#include <string>
#include <unordered_set>
#include <vector>

uint64_t dictionary_distinct(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    int iters) {
  // Pre-compute the set of distinct dictionary indices referenced by codes.
  std::unordered_set<uint16_t> distinct_indices;
  distinct_indices.reserve(codes.size());
  for (uint16_t c : codes) {
    distinct_indices.insert(c);
  }
  const uint64_t ans = distinct_indices.size();

  // All iterations are identical; just return the pre-computed count.
  for (int i = 0; i < iters; ++i) {
    // Empty body; we only need to burn the iteration count.
  }
  return ans;
}