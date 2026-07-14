#include "interface.h"

#include <string>
#include <vector>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  (void)iters;

  const std::size_t dict_size = dictionary.size();
  std::vector<uint8_t> match(dict_size);
  for (std::size_t i = 0; i < dict_size; ++i) {
    const std::string& s = dictionary[i];
    match[i] = (s.size() > 1 && s[0] == 'q' && s[1] <= 'm') ? 1u : 0u;
  }

  uint64_t sum = 0;
  const std::size_t n = codes.size();

  const uint16_t* cptr = codes.data();
  const uint32_t* wptr = weights.data();

  std::size_t i = 0;
  const std::size_t n4 = n / 4;
  for (std::size_t k = 0; k < n4; ++k) {
    uint16_t c0 = cptr[0];
    uint16_t c1 = cptr[1];
    uint16_t c2 = cptr[2];
    uint16_t c3 = cptr[3];
    if (match[c0]) sum += wptr[0];
    if (match[c1]) sum += wptr[1];
    if (match[c2]) sum += wptr[2];
    if (match[c3]) sum += wptr[3];
    cptr += 4;
    wptr += 4;
  }
  i = n4 * 4;
  for (; i < n; ++i) {
    uint16_t c = *cptr++;
    if (match[c]) sum += *wptr;
    ++wptr;
  }

  return sum;
}
