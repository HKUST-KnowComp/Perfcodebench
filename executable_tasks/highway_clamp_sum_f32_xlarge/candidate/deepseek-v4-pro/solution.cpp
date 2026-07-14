#include "interface.h"
#include <hwy/highway.h>

namespace hn = hwy::HWY_NAMESPACE;

float clamp_sum(const float* data, std::size_t n) {
  using namespace hn;
  const ScalableTag<float> d;
  const size_t N = Lanes(d);

  auto sum0 = Zero(d);
  auto sum1 = Zero(d);
  auto sum2 = Zero(d);
  auto sum3 = Zero(d);
  const auto zero = Zero(d);

  size_t i = 0;
  // Process 4 vectors per iteration for better pipelining
  for (; i + 4 * N <= n; i += 4 * N) {
    auto v0 = LoadU(d, data + i);
    auto v1 = LoadU(d, data + i + N);
    auto v2 = LoadU(d, data + i + 2 * N);
    auto v3 = LoadU(d, data + i + 3 * N);

    sum0 = Add(sum0, IfThenElse(Gt(v0, zero), v0, zero));
    sum1 = Add(sum1, IfThenElse(Gt(v1, zero), v1, zero));
    sum2 = Add(sum2, IfThenElse(Gt(v2, zero), v2, zero));
    sum3 = Add(sum3, IfThenElse(Gt(v3, zero), v3, zero));
  }

  // Handle remaining full vectors into sum0
  for (; i + N <= n; i += N) {
    auto v = LoadU(d, data + i);
    sum0 = Add(sum0, IfThenElse(Gt(v, zero), v, zero));
  }

  // Combine accumulators
  sum0 = Add(Add(sum0, sum1), Add(sum2, sum3));

  // Horizontal reduction
  float total = ReduceSum(d, sum0);

  // Scalar tail (handles any remaining elements and avoids alignment issues)
  for (; i < n; ++i) {
    total += data[i] > 0.0f ? data[i] : 0.0f;
  }

  return total;
}
