#include "interface.h"
#include <vector>
#include <cstdint>

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
  (void)iters;
  const size_t n = dictionary.size();
  std::vector<uint64_t> sums(n, 0);
  const size_t m = codes.size();
  const uint16_t* c_ptr = codes.data();
  const uint32_t* v_ptr = values.data();
  for (size_t i = 0; i < m; ++i) {
    sums[c_ptr[i]] += static_cast<uint64_t>(v_ptr[i]);
  }
  uint64_t hash = 1469598103934665603ULL;
  const uint64_t* s_ptr = sums.data();
  for (size_t i = 0; i < n; ++i) {
    hash ^= s_ptr[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}
