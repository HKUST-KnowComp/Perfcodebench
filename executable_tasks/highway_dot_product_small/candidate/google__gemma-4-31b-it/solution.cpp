#include "interface.h"
#include <hwy/highway.h>

namespace hw = hwy;

float dot_product(const float* a, const float* b, std::size_t n) {
    if (n == 0) return 0.0f;

    const hw::ScalableTag<float> d;
    auto v_sum = hw::Zero(d);
    
    std::size_t i = 0;
    const std::size_t lanes = hw::Lanes(d);

    // Vectorized loop
    for (; i + lanes <= n; i += lanes) {
        auto va = hw::Load(d, a + i);
        auto vb = hw::Load(d, b + i);
        v_sum = hw::Add(v_sum, hw::Mul(va, vb));
    }

    // Horizontal reduction of the SIMD register
    float total_sum = hw::ReduceSum(d, v_sum);

    // Scalar tail for remaining elements
    for (; i < n; ++i) {
        total_sum += a[i] * b[i];
    }

    return total_sum;
}