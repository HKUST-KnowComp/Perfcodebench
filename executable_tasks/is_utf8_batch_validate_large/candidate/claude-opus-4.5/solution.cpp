#include "interface.h"
#include <is_utf8.h>

std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  const std::size_t n = inputs.size();
  
  #pragma omp parallel for reduction(+:count) schedule(dynamic, 64)
  for (std::size_t i = 0; i < n; ++i) {
    const std::string& s = inputs[i];
    if (is_utf8(s.data(), s.size())) {
      ++count;
    }
  }
  
  return count;
}