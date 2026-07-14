#include "interface.h"
#include <hwy/highway.h>

namespace hw = hwy::HWY_NAMESPACE;

float clamp_sum(const float* data, std::size_t n) {
  const hw::ScalableTag<float> d;
  const auto zero = hw::Set(d, 0.0f);
  auto sum_vec = hw::Zero(d);

  std::size_t i = 0;
  for (; i <= n - hw::Lanes(d); i += hw::Lanes(d)) {
    auto v = hw::Load(d, data + i);
    auto clamped = hw::Max(v, zero);
    sum_vec = hw::Add(sum_vec, clamped);
  }

  float total_sum = hw::ReduceSum(d, sum_vec);

  for (; i < n; ++i) {
    if (data[i] > 0.0f) {
      total_sum += data[i];
    }
  }

  return total_sum;
}