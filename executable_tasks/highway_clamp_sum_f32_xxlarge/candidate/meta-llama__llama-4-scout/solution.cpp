#include "interface.h"
#include <hwy/highway.h>

float clamp_sum(const float* data, std::size_t n) {
  float sum = 0.0f;
  const auto h = hwy::Highway<float, hwy::ScalableTag>();
  const auto lane_count = h.LaneCount();
  const auto zero = h.Set(0.0f);
  for (std::size_t i = 0; i < n; i += lane_count) {
    const auto values = h.LoadU(data + i);
    const auto clamped = h.Max(zero, values);
    sum += h.SumOfLanes(clamped);
  }
  return sum;
}