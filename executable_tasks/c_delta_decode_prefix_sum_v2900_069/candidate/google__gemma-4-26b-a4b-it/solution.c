#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  // The original code allocates 'values' but the logic inside the loop 
  // can be fused to avoid the extra pass and the extra memory allocation.
  // The original code's 'total' is reset every iteration, so only the 
  // result of the last iteration matters.
  
  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    uint64_t iteration_total = 0;
    for (size_t i = 0; i < count; ++i) {
      current += (int64_t)deltas[i];
      iteration_total += (uint64_t)(current + 0x9E3779B1LL);
    }
    total = iteration_total;
  }
  
  return total;
}