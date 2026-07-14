#include "interface.h"
#include <hwy/highway.h>
#include <cstddef>

HWY_BEFORE_NAMESPACE();
namespace hwy_clamp_sum {
namespace HWY_NAMESPACE {

float clamp_sum_impl(const float* data, std::size_t n) {
    const ScalableTag<float> d;
    const auto zero = Zero(d);
    auto sum = Zero(d);
    std::size_t i = 0;
    const std::size_t lanes = Lanes(d);
    
    for (; i + lanes <= n; i += lanes) {
        auto v = Load(d, data + i);
        v = Max(v, zero);
        sum = Add(sum, v);
    }
    
    float total = ReduceSum(d, sum);
    
    for (; i < n; ++i) {
        float val = data[i];
        if (val > 0.0f) total += val;
    }
    
    return total;
}

} // namespace HWY_NAMESPACE
} // namespace hwy_clamp_sum
HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace hwy_clamp_sum {
HWY_EXPORT(clamp_sum_impl);
}

float clamp_sum(const float* data, std::size_t n) {
    return HWY_DYNAMIC_DISPATCH(hwy_clamp_sum::clamp_sum_impl)(data, n);
}
#endif