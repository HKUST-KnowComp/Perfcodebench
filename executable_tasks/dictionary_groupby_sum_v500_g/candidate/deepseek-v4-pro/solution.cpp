#include "interface.h"

#include <algorithm>
#include <cstdint>
#include <string>
#include <unordered_map>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t dictionary_groupby_checksum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& values,
    int iters) {
  if (iters <= 0) {
    return 0;
  }

  // Map each distinct string in dictionary to a canonical accumulator index.
  std::unordered_map<std::string, std::size_t> str_to_id;
  std::vector<std::size_t> canonical_idx(dictionary.size());
  for (std::size_t i = 0; i < dictionary.size(); ++i) {
    const std::string& s = dictionary[i];
    auto it = str_to_id.find(s);
    if (it == str_to_id.end()) {
      std::size_t id = str_to_id.size();
      str_to_id[s] = id;
      canonical_idx[i] = id;
    } else {
      canonical_idx[i] = it->second;
    }
  }

  const std::size_t num_distinct = str_to_id.size();
  std::vector<uint64_t> accumulators(num_distinct, 0);

  // Precompute canonical index for each code to avoid dictionary indirection inside the hot loop.
  std::vector<std::size_t> code_canonical_idx(codes.size());
  for (std::size_t i = 0; i < codes.size(); ++i) {
    code_canonical_idx[i] = canonical_idx[codes[i]];
  }

  const std::size_t N = codes.size();
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    // Zero accumulators.
    std::fill(accumulators.begin(), accumulators.end(), uint64_t(0));

    // Aggregate values by canonical ID.
    for (std::size_t i = 0; i < N; ++i) {
      accumulators[code_canonical_idx[i]] += static_cast<uint64_t>(values[i]);
    }

    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < dictionary.size(); ++i) {
      hash = mix(hash, accumulators[canonical_idx[i]]);
    }
  }

  return hash;
}
