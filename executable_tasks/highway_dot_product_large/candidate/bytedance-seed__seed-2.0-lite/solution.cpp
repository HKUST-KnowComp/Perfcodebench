#include "interface.h"
#include <hwy/highway.h>

float dot_product(const float* a, const float* b, std::size_t n) {
    const hwy::ScalableTag<float> d{};
    const size_t lanes = hwy::Lanes(d);
    auto sum_vec = hwy::Zero(d);

    size_t i = 0;
    // Process all full SIMD vector blocks for maximum throughput
    for (; i + lanes <= n; i += lanes) {
        const auto va = hwy::Load(d, a + i);
        const auto vb = hwy::Load(d, b + i);
        sum_vec += va * vb;
    }

    // Reduce vector accumulator to scalar, add remaining elements
    float total = hwy::ReduceSum(d, sum_vec);
    for (; i < n; ++i) {
        total += a[i] * b[i];
    }

    return total;
}