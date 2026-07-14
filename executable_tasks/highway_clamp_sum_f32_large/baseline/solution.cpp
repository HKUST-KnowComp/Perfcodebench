#include "interface.h"

float clamp_sum(const float* data, std::size_t n) {
  float sum = 0.0f;
  for (std::size_t i = 0; i < n; ++i) {
    sum += data[i] > 0.0f ? data[i] : 0.0f;
  }
  return sum;
}
