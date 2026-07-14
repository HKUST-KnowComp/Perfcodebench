#include "interface.h"
#include <hwy/highway.h>

float clamp_sum(const float* data, std::size_t n) {
  using namespace hwy::HWY_NAMESPACE;
  const ScalableTag<float> df;
  const auto lanes = Lanes(df);

  size_t i = 0;
  auto sum0 = Zero(df);
  auto sum1 = Zero(df);
  auto sum2 = Zero(df);
  auto sum3 = Zero(df);

  for (; i + 4 * lanes <= n; i += 4 * lanes) {
    auto v0 = LoadU(df, data + i);
    auto v1 = LoadU(df, data + i + lanes);
    auto v2 = LoadU(df, data + i + 2 * lanes);
    auto v3 = LoadU(df, data + i + 3 * lanes);
    sum0 = Add(sum0, Max(v0, Zero(df)));
    sum1 = Add(sum1, Max(v1, Zero(df)));
    sum2 = Add(sum2, Max(v2, Zero(df)));
    sum3 = Add(sum3, Max(v3, Zero(df)));
  }

  for (; i + lanes <= n; i += lanes) {
    auto v = LoadU(df, data + i);
    sum0 = Add(sum0, Max(v, Zero(df)));
  }

  auto sum_vector = Add(Add(sum0, sum1), Add(sum2, sum3));
  float sum = GetLane(SumOfLanes(df, sum_vector));

  for (; i < n; ++i) {
    float val = data[i];
    sum += val > 0.0f ? val : 0.0f;
  }

  return sum;
}