#include "interface.h"
#include <hwy/highway.h>

namespace hw = hwy;

float clamp_sum(const float* data, std::size_t n) {
    if (n == 0) return 0.0f;

    const hw::ScalableTag<float> d;
    auto zero = hw::Set(d, 0.0f);
    auto sum_vec = hw::Zero(d);

    const float* ptr = data;
    std::size_t i = 0;

    // Process in SIMD chunks
    for (; i <= n - hw::Lanes(d); i += hw::Lanes(d)) {
        auto v = hw::Load(d, ptr + i);
        // Clamp: max(0.0f, value)
        auto clamped = hw::Max(v, zero);
        sum_vec = hw::Add(sum_vec, clamped);
    }

    // Horizontal sum of the vector
    float total_sum = hw::ReduceSum(d, sum_vec);

    // Handle remaining elements
    for (; i < n; ++i) {
        if (ptr[i] > 0.0f) {
            total_sum += ptr[i];
        }
    }

    return total_sum;
}