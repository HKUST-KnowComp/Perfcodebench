#include "interface.h"
#include <hwy/highway.h>

namespace hw = hwy;

float dot_product(const float* a, const float* b, std::size_t n) {
    if (n == 0) return 0.0f;

    const hw::HWF32D d = hw::Simd<float>{};
    hw::V32F sum_vec = hw::Zero(d);
    
    std::size_t i = 0;
    // Process in blocks of 4 vectors to improve instruction-level parallelism
    for (; i + 4 * hw::Lanes(d) <= n; i += 4 * hw::Lanes(d)) {
        hw::V32F a0 = hw::Load(d, a + i);
        hw::V32F b0 = hw::Load(d, b + i);
        sum_vec = hw::Add(sum_vec, hw::Mul(a0, b0));

        hw::V32F a1 = hw::Load(d, a + i + hw::Lanes(d));
        hw::V32F b1 = hw::Load(d, b + i + hw::Lanes(d));
        sum_vec = hw::Add(sum_vec, hw::Mul(a1, b1));

        hw::V32F a2 = hw::Load(d, a + i + 2 * hw::Lanes(d));
        hw::V32F b2 = hw::Load(d, b + i + 2 * hw::Lanes(d));
        sum_vec = hw::Add(sum_vec, hw::Mul(a2, b2));

        hw::V32F a3 = hw::Load(d, a + i + 3 * hw::Lanes(d));
        hw::V32F b3 = hw::Load(d, b + i + 3 * hw::Lanes(d));
        sum_vec = hw::Add(sum_vec, hw::Mul(a3, b3));
    }

    // Process remaining full vectors
    for (; i + hw::Lanes(d) <= n; i += hw::Lanes(d)) {
        hw::V32F va = hw::Load(d, a + i);
        hw::V32F vb = hw::Load(d, b + i);
        sum_vec = hw::Add(sum_vec, hw::Mul(va, vb));
    }

    // Horizontal sum of the SIMD register
    float total_sum = hw::ReduceSum(sum_vec);

    // Tail processing for remaining elements
    for (; i < n; ++i) {
        total_sum += a[i] * b[i];
    }

    return total_sum;
}