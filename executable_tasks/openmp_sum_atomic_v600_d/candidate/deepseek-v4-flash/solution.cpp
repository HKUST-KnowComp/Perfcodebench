#include "interface.h"
#include <cstdint>
#include <omp.h>

uint64_t sum_repeated(const std::vector<uint32_t>& input, int /* iters */) {
  uint64_t sum = 0;
  #pragma omp parallel for reduction(+:sum) schedule(static)
  for (std::size_t i = 0; i < input.size(); ++i) {
    sum += static_cast<uint64_t>(input[i]);
  }
  return sum;
}
