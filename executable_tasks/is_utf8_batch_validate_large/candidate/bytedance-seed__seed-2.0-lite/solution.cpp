#include "interface.h"
#include "is_utf8.h"

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  const size_t num_inputs = inputs.size();
  for (size_t i = 0; i < num_inputs; ++i) {
    // Prefetch next string's first cache line to reduce memory latency for large batches
    if (i + 1 < num_inputs) {
      __builtin_prefetch(inputs[i+1].data(), 0, 0);
    }
    const auto& s = inputs[i];
    if (is_utf8(s.data(), s.size())) {
      ++count;
    }
  }
  return count;
}