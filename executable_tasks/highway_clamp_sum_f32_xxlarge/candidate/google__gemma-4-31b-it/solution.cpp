#include "interface.h"
#include <hwy/highway.h>

namespace hw = hwy::HWY_NAMESPACE;

float clamp_sum(const float* data, std::size_t n) {
    if (n == 0) return 0.0f;

    const hw::ScalableTag<float> d;
    auto v_zero = hw::Set(d, 0.0f);
    auto v_sum = hw::Zero(d);

    std::size_t i = 0;
    for (; i <= n - hw::Lanes(d); i += hw::Lanes(d)) {
        auto v_data = hw::Load(d, data + i);
        // Clamp: max(0.0f, value)
        auto v_clamped = hw::Max(v_data, v_zero);
        v_sum = hw::Add(v_sum, v_clamped);
    }

    float total_sum = hw::ReduceSum(d, v_sum);

    // Handle tail
    for (; i < n; ++i) {
        if (data[i] > 0.0f) {
            total_sum += data[i];
        }
    }

    return total_sum;
}