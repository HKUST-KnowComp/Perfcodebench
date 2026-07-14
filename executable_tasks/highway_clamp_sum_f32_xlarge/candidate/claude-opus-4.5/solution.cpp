#include "interface.h"
#include <hwy/highway.h>

HWY_BEFORE_NAMESPACE();
namespace hwy_clamp {
namespace HWY_NAMESPACE {

namespace hn = hwy::HWY_NAMESPACE;

float clamp_sum_impl(const float* HWY_RESTRICT data, std::size_t n) {
    const hn::ScalableTag<float> d;
    const std::size_t N = hn::Lanes(d);
    const auto zero = hn::Zero(d);
    
    auto sum0 = zero;
    auto sum1 = zero;
    auto sum2 = zero;
    auto sum3 = zero;
    
    std::size_t i = 0;
    const std::size_t limit4 = (n >= 4 * N) ? (n - 4 * N + 1) : 0;
    
    for (; i < limit4; i += 4 * N) {
        const auto v0 = hn::LoadU(d, data + i);
        const auto v1 = hn::LoadU(d, data + i + N);
        const auto v2 = hn::LoadU(d, data + i + 2 * N);
        const auto v3 = hn::LoadU(d, data + i + 3 * N);
        
        sum0 = hn::Add(sum0, hn::Max(v0, zero));
        sum1 = hn::Add(sum1, hn::Max(v1, zero));
        sum2 = hn::Add(sum2, hn::Max(v2, zero));
        sum3 = hn::Add(sum3, hn::Max(v3, zero));
    }
    
    for (; i + N <= n; i += N) {
        const auto v = hn::LoadU(d, data + i);
        sum0 = hn::Add(sum0, hn::Max(v, zero));
    }
    
    sum0 = hn::Add(sum0, sum1);
    sum2 = hn::Add(sum2, sum3);
    sum0 = hn::Add(sum0, sum2);
    
    float result = hn::ReduceSum(d, sum0);
    
    for (; i < n; ++i) {
        const float val = data[i];
        result += (val > 0.0f) ? val : 0.0f;
    }
    
    return result;
}

}  // namespace HWY_NAMESPACE
}  // namespace hwy_clamp
HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace hwy_clamp {
HWY_EXPORT(clamp_sum_impl);
}

float clamp_sum(const float* data, std::size_t n) {
    return hwy_clamp::HWY_DYNAMIC_DISPATCH(clamp_sum_impl)(data, n);
}
#endif