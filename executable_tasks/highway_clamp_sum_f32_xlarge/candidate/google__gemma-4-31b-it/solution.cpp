#include "interface.h"
#include <hwy/highway.h>

namespace hw = hwy::HWY_NAMESPACE;

float clamp_sum(const float* data, std::size_t n) {
    if (n == 0) return 0.0f;

    const hw::ScalableTag<float> d;
    auto zero = hw::Set(d, 0.0f);
    auto sum_vec = hw::Zero(d);

    std::size_t i = 0;
    for (; i + hw::Lanes(d) <= n; i += hw::Lanes(d)) {
        auto v = hw::Load(d, data + i);
        sum_vec = hw::Add(sum_vec, hw::Max(v, zero));
    }

    float total_sum = hw::ReduceSum(sum_vec);

    for (; i < n; ++i) {
        if (data[i] > 0.0f) {
            total_sum += data[i];
        }
    }

    return total_sum;
}