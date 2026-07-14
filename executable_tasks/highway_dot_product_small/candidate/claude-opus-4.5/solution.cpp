#include "interface.h"
#include <hwy/highway.h>

HWY_BEFORE_NAMESPACE();
namespace hwy_dot {
namespace HWY_NAMESPACE {

namespace hn = hwy::HWY_NAMESPACE;

float dot_product_impl(const float* HWY_RESTRICT a, const float* HWY_RESTRICT b, std::size_t n) {
    const hn::ScalableTag<float> d;
    const std::size_t N = hn::Lanes(d);
    
    auto sum0 = hn::Zero(d);
    auto sum1 = hn::Zero(d);
    auto sum2 = hn::Zero(d);
    auto sum3 = hn::Zero(d);
    
    std::size_t i = 0;
    const std::size_t unroll = N * 4;
    
    for (; i + unroll <= n; i += unroll) {
        const auto a0 = hn::LoadU(d, a + i);
        const auto b0 = hn::LoadU(d, b + i);
        sum0 = hn::MulAdd(a0, b0, sum0);
        
        const auto a1 = hn::LoadU(d, a + i + N);
        const auto b1 = hn::LoadU(d, b + i + N);
        sum1 = hn::MulAdd(a1, b1, sum1);
        
        const auto a2 = hn::LoadU(d, a + i + N * 2);
        const auto b2 = hn::LoadU(d, b + i + N * 2);
        sum2 = hn::MulAdd(a2, b2, sum2);
        
        const auto a3 = hn::LoadU(d, a + i + N * 3);
        const auto b3 = hn::LoadU(d, b + i + N * 3);
        sum3 = hn::MulAdd(a3, b3, sum3);
    }
    
    for (; i + N <= n; i += N) {
        const auto va = hn::LoadU(d, a + i);
        const auto vb = hn::LoadU(d, b + i);
        sum0 = hn::MulAdd(va, vb, sum0);
    }
    
    sum0 = hn::Add(sum0, sum1);
    sum2 = hn::Add(sum2, sum3);
    sum0 = hn::Add(sum0, sum2);
    
    float result = hn::ReduceSum(d, sum0);
    
    for (; i < n; ++i) {
        result += a[i] * b[i];
    }
    
    return result;
}

}  // namespace HWY_NAMESPACE
}  // namespace hwy_dot
HWY_AFTER_NAMESPACE();

#if HWY_ONCE
namespace hwy_dot {
HWY_EXPORT(dot_product_impl);
}

float dot_product(const float* a, const float* b, std::size_t n) {
    return hwy_dot::HWY_DYNAMIC_DISPATCH(dot_product_impl)(a, b, n);
}
#endif
