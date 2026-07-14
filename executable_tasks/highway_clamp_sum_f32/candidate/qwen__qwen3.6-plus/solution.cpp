#include "interface.h"
#include "hwy/highway.h"

HWY_BEFORE_NAMESPACE();
namespace hwy_clamp {
namespace HWY_NAMESPACE {

float ClampSumImpl(const float* data, std::size_t n) {
    const hwy::ScalableTag<float> d;
    const auto zero = hwy::Zero(d);
    auto sum = hwy::Zero(d);
    size_t i = 0;
    for (; i + hwy::Lanes(d) <= n; i += hwy::Lanes(d)) {
        auto v = hwy::Load(d, data + i);
        v = hwy::Max(v, zero);
        sum = hwy::Add(sum, v);
    }
    float total = hwy::ReduceSum(d, sum);
    for (; i < n; ++i) {
        total += data[i] > 0.0f ? data[i] : 0.0f;
    }
    return total;
}

} // namespace HWY_NAMESPACE
} // namespace hwy_clamp
HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace hwy_clamp {
HWY_EXPORT(ClampSumImpl);
}

float clamp_sum(const float* data, std::size_t n) {
    return HWY_DYNAMIC_DISPATCH(hwy_clamp::ClampSumImpl)(data, n);
}
#endif