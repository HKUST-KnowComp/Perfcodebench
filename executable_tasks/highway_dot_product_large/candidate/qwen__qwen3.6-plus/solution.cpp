#include "interface.h"
#include <cstddef>
#include <hwy/highway.h>

HWY_BEFORE_NAMESPACE();
namespace hwy_dot {
namespace HWY_NAMESPACE {

float ComputeDotProduct(const float* a, const float* b, size_t n) {
    const hwy::ScalableTag<float> d;
    auto sum = hwy::Zero(d);
    size_t i = 0;
    const size_t lanes = hwy::Lanes(d);
    for (; i + lanes <= n; i += lanes) {
        sum = hwy::MulAdd(hwy::LoadU(d, a + i), hwy::LoadU(d, b + i), sum);
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
HWY_EXPORT(ComputeDotProduct);
float dot_product(const float* a, const float* b, size_t n) {
    return HWY_DYNAMIC_DISPATCH(ComputeDotProduct)(a, b, n);
}
} // namespace hwy_dot
#endif