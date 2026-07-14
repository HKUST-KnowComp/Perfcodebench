#include "interface.h"
#include <hwy/highway.h>

namespace hw = hwy::HWY_NAMESPACE;

float dot_product(const float* a, const float* b, std::size_t n) {
    const hw::ScalableTag<float> d;
    const hw::constexpr_simd_t<d> zero = hw::Zero(d);
    hw::Simd<d> sum = zero;

    std::size_t i = 0;
    // Process elements in SIMD chunks
    for (; i <= n - hw::Lanes(d); i += hw::Lanes(d)) {
        hw::Simd<d> va = hw::Load(d, a + i);
        hw::Simd<d> vb = hw::Load(d, b + i);
        sum = hw::Add(sum, hw::Mul(va, vb));
    }

    // Horizontal sum of the SIMD register
    float total_sum = hw::ReduceSum(d, sum);

    // Handle remaining elements
    for (; i < n; ++i) {
        total_sum += a[i] * b[i];
    }

    return total_sum;
}