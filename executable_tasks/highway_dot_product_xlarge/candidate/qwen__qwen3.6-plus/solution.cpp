#include <hwy/highway.h>
#include <cstddef>

HWY_BEFORE_NAMESPACE();
namespace HWY_NAMESPACE {
namespace hn = hwy::HWY_NAMESPACE;

float DotProduct(const float* a, const float* b, std::size_t n) {
    const hn::ScalableTag<float> d;
    auto sum = hn::Zero(d);
    std::size_t i = 0;
    const std::size_t lanes = hn::Lanes(d);
    for (; i + lanes <= n; i += lanes) {
        sum = hn::MulAdd(hn::LoadU(d, a + i), hn::LoadU(d, b + i), sum);
    }
    float res = hn::ReduceSum(d, sum);
    for (; i < n; ++i) {
        res += a[i] * b[i];
    }
    return res;
}

} // namespace HWY_NAMESPACE
HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace {
HWY_EXPORT(DotProduct);
}

float dot_product(const float* a, const float* b, std::size_t n) {
    return HWY_DYNAMIC_DISPATCH(DotProduct)(a, b, n);
}
#endif