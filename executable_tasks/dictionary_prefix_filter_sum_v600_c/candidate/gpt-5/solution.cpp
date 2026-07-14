#include "interface.h"

#include <string>
#include <vector>

namespace {
inline uint8_t matches_prefix_u8(const std::string& s) {
  // Safe check avoiding out-of-bounds for short strings
  return (s.size() >= 2 && s[0] == 'q' && s[1] <= 'm') ? 1u : 0u;
}
}  // namespace

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int /*iters*/) {
  // Precompute which dictionary entries match the prefix rule
  const std::size_t dict_size = dictionary.size();
  std::vector<uint8_t> mask(dict_size);
  for (std::size_t i = 0; i < dict_size; ++i) {
    mask[i] = matches_prefix_u8(dictionary[i]);
  }

  // Sum weights for rows whose decoded string matches, without materializing decoded strings
  const std::size_t n = codes.size();
  uint64_t sum = 0;

  if (n == 0) return sum;

  const uint16_t* __restrict codes_ptr = codes.data();
  const uint32_t* __restrict weights_ptr = weights.data();
  const uint8_t* __restrict mask_ptr = mask.data();

  std::size_t i = 0;
  const std::size_t n4 = n & ~std::size_t(3);
  for (; i < n4; i += 4) {
    const uint16_t c0 = codes_ptr[i];
    const uint16_t c1 = codes_ptr[i + 1];
    const uint16_t c2 = codes_ptr[i + 2];
    const uint16_t c3 = codes_ptr[i + 3];

    sum += static_cast<uint64_t>(weights_ptr[i]) * mask_ptr[c0];
    sum += static_cast<uint64_t>(weights_ptr[i + 1]) * mask_ptr[c1];
    sum += static_cast<uint64_t>(weights_ptr[i + 2]) * mask_ptr[c2];
    sum += static_cast<uint64_t>(weights_ptr[i + 3]) * mask_ptr[c3];
  }
  for (; i < n; ++i) {
    sum += static_cast<uint64_t>(weights_ptr[i]) * mask_ptr[codes_ptr[i]];
  }

  return sum;
}
