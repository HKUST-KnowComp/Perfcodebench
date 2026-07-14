#include "interface.h"

float dot_product(const float* a, const float* b, std::size_t n) {
  float sum = 0.0f;
  for (std::size_t i = 0; i < n; ++i) {
    sum += a[i] * b[i];
  }
  return sum;
}
