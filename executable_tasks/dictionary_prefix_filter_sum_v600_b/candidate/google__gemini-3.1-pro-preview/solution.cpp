#include "interface.h"

#include <string>
#include <vector>
#include <cstdint>

uint64_t dictionary_prefix_weight_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights,
    int iters) {
  if (iters <= 0) return 0;

  std::vector<uint8_t> valid_code(dictionary.size(), 0);
  for (size_t i = 0; i < dictionary.size(); ++i) {
    const std::string& s = dictionary[i];
    if (!s.empty() && s[0] == 'q') {
      char c1 = (s.size() > 1) ? s[1] : '\0';
      if (c1 <= 'm') {
        valid_code[i] = 1;
      }
    }
  }

  uint64_t sum = 0;
  size_t n = codes.size();
  const uint16_t* c_ptr = codes.data();
  const uint32_t* w_ptr = weights.data();
  const uint8_t* v_ptr = valid_code.data();

  for (size_t i = 0; i < n; ++i) {
    sum += static_cast<uint64_t>(w_ptr[i]) * v_ptr[c_ptr[i]];
  }

  return sum;
}
