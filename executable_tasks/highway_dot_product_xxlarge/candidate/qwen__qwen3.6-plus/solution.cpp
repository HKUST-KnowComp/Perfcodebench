#include "interface.h"
#include <hwy/highway.h>

HWY_BEFORE_NAMESPACE();
namespace hwy_dot {
namespace HWY_NAMESPACE {

float ComputeDot(const float* a, const float* b, std::size_t n) {
    const hwy::ScalableTag<float> d;
    auto sum = hwy::Zero(d);
    std::size_t i = 0;
    const std::size_t lanes = hwy::Lanes(d);
    for (; i + lanes <= n; i += lanes) {
        auto va = hwy::LoadU(d, a + i);
        auto vb = hwy::LoadU(d, b + i);
        sum = hwy::MulAdd(va, vb, sum);
    }
    float res = hwy::ReduceSum(d, sum);
    for (; i < n; ++i) {
        res += a[i] * b[i];
    }
    return res;
}

} // namespace HWY_NAMESPACE
} // namespace hwy_dot
HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace hwy_dot {
HWY_EXPORT(ComputeDot);
float dot_product(const float* a, const float* b, std::size_t n) {
    return HWY_DYNAMIC_DISPATCH(ComputeDot)(a, b, n);
}
} // namespace hwy_dot
#endif