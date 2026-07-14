#include "interface.h"
#include <hwy/highway.h>

float dot_product(const float* a, const float* b, std::size_t n) {
    using namespace hwy::HWY_NAMESPACE;
    const ScalableTag<float> d;
    const size_t N = Lanes(d);

    auto sum0 = Zero(d);
    auto sum1 = Zero(d);
    auto sum2 = Zero(d);
    auto sum3 = Zero(d);

    size_t i = 0;
    // Process 4 vectors per iteration to hide FMA latency
    for (; i + 4 * N <= n; i += 4 * N) {
        sum0 = MulAdd(LoadU(d, a + i), LoadU(d, b + i), sum0);
        sum1 = MulAdd(LoadU(d, a + i + N), LoadU(d, b + i + N), sum1);
        sum2 = MulAdd(LoadU(d, a + i + 2 * N), LoadU(d, b + i + 2 * N), sum2);
        sum3 = MulAdd(LoadU(d, a + i + 3 * N), LoadU(d, b + i + 3 * N), sum3);
    }

    // Combine accumulators
    sum0 = Add(sum0, sum1);
    sum2 = Add(sum2, sum3);
    sum0 = Add(sum0, sum2);

    // Process remaining full vectors
    for (; i + N <= n; i += N) {
        sum0 = MulAdd(LoadU(d, a + i), LoadU(d, b + i), sum0);
    }

    float total = ReduceSum(d, sum0);

    // Scalar tail
    for (; i < n; ++i) {
        total += a[i] * b[i];
    }

    return total;
}