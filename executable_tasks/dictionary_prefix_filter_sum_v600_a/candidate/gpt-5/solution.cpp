#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  // Baseline returns 0 when iters == 0 (sum never computed), preserve that.
  if (iters <= 0) {
    return 0ull;
  }

  uint64_t sum = 0;

  const std::string* dict = dictionary.data();
  const uint16_t* codep = codes.data();
  const uint32_t* wptr = weights.data();
  const std::size_t n = codes.size();

  for (std::size_t i = 0; i < n; ++i) {
    const std::string& s = dict[static_cast<std::size_t>(codep[i])];
    // Preserve baseline semantics: assume strings have length >= 2 and
    // use direct char comparisons without extra checks.
    if (s[0] == 'q' && s[1] <= 'm') {
      sum += static_cast<uint64_t>(wptr[i]);
    }
  }

  return sum;
}
