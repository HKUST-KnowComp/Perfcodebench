#include "interface.h"

#include <stdint.h>

#define STACK_ALLOC_SIZE 1024

uint64_t run(const int32_t* deltas, size_t count, int iters) {
  uint64_t total = 0;
  int64_t values[STACK_ALLOC_SIZE];
  if (count > STACK_ALLOC_SIZE) return 0;
  for (int iter = 0; iter < iters; ++iter) {
    int64_t current = 0;
    for (size_t i = 0; i < count; ++i) {
      current += deltas[i];
      values[i] = current;
    }
    total = 0;
    for (size_t i = 0; i < count; ++i) {
      total += values[i] + 0x9E3779B1LL;
    }
  }
  return total;
}