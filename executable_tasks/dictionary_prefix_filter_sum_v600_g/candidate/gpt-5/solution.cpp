#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

namespace {
inline uint8_t matches_prefix_fast(const std::string& s) {
  if (s.size() < 2) return 0;
  return static_cast<uint8_t>(s[0] == 'q' && s[1] <= 'm');
}
}

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int /*iters*/) {
  const std::size_t n = codes.size();
  const std::size_t dict_sz = dictionary.size();

  uint64_t sum = 0;

  // Heuristic: if dictionary is relatively small compared to number of rows,
  // precompute match flags for all dictionary entries; otherwise, compute lazily
  // for only the dictionary indices actually referenced by codes.
  if (dict_sz * 2 <= n) {
    std::vector<uint8_t> flags(dict_sz);
    for (std::size_t i = 0; i < dict_sz; ++i) {
      flags[i] = matches_prefix_fast(dictionary[i]);
    }

    const uint8_t* f = flags.data();
    const uint16_t* c = codes.data();
    const uint32_t* w = weights.data();

    std::size_t i = 0;
    std::size_t n4 = n & ~static_cast<std::size_t>(3);
    for (; i < n4; i += 4) {
      sum += static_cast<uint64_t>(w[i + 0]) * f[c[i + 0]];
      sum += static_cast<uint64_t>(w[i + 1]) * f[c[i + 1]];
      sum += static_cast<uint64_t>(w[i + 2]) * f[c[i + 2]];
      sum += static_cast<uint64_t>(w[i + 3]) * f[c[i + 3]];
    }
    for (; i < n; ++i) {
      sum += static_cast<uint64_t>(w[i]) * f[c[i]];
    }
  } else {
    // Lazy computation: initialize flags to -1 (unknown), compute on first use.
    std::vector<int8_t> flags(dict_sz, static_cast<int8_t>(-1));
    const uint16_t* c = codes.data();
    const uint32_t* w = weights.data();

    for (std::size_t i = 0; i < n; ++i) {
      const std::size_t idx = static_cast<std::size_t>(c[i]);
      int8_t v = flags[idx];
      if (v < 0) {
        v = static_cast<int8_t>(matches_prefix_fast(dictionary[idx]));
        flags[idx] = v;
      }
      sum += static_cast<uint64_t>(w[i]) * static_cast<uint8_t>(v);
    }
  }

  return sum;
}
