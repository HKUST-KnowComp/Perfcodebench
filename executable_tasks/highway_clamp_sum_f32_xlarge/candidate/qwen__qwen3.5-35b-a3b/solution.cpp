#include "interface.h"
#include "hwy/highway.h"

float clamp_sum(const float* data, std::size_t n) {
  using namespace hwy;
  const DFromT<float> d;
  const size_t num_lanes = Lanes(d);
  float sum = 0.0f;

  for (size_t i = 0; i + num_lanes <= n; i += num_lanes) {
    auto vec = Load(d, data + i);
    auto clamped = Max(Zero(d), vec);
    sum += SumOfLanes(d, clamped);
  }

  for (size_t i = n - (n % num_lanes); i < n; ++i) {
    sum += data[i] > 0.0f ? data[i] : 0.0f;
  }

  return sum;
}