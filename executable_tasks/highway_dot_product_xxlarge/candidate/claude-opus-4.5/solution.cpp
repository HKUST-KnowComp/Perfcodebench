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
    auto sum4 = hn::Zero(d);
    auto sum5 = hn::Zero(d);
    auto sum6 = hn::Zero(d);
    auto sum7 = hn::Zero(d);
    
    std::size_t i = 0;
    const std::size_t unroll = N * 8;
    
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
        
        const auto a4 = hn::LoadU(d, a + i + N * 4);
        const auto b4 = hn::LoadU(d, b + i + N * 4);
        sum4 = hn::MulAdd(a4, b4, sum4);
        
        const auto a5 = hn::LoadU(d, a + i + N * 5);
        const auto b5 = hn::LoadU(d, b + i + N * 5);
        sum5 = hn::MulAdd(a5, b5, sum5);
        
        const auto a6 = hn::LoadU(d, a + i + N * 6);
        const auto b6 = hn::LoadU(d, b + i + N * 6);
        sum6 = hn::MulAdd(a6, b6, sum6);
        
        const auto a7 = hn::LoadU(d, a + i + N * 7);
        const auto b7 = hn::LoadU(d, b + i + N * 7);
        sum7 = hn::MulAdd(a7, b7, sum7);
    }
    
    // Process remaining full vectors
    for (; i + N <= n; i += N) {
        const auto av = hn::LoadU(d, a + i);
        const auto bv = hn::LoadU(d, b + i);
        sum0 = hn::MulAdd(av, bv, sum0);
    }
    
    // Combine all accumulators
    sum0 = hn::Add(sum0, sum1);
    sum2 = hn::Add(sum2, sum3);
    sum4 = hn::Add(sum4, sum5);
    sum6 = hn::Add(sum6, sum7);
    sum0 = hn::Add(sum0, sum2);
    sum4 = hn::Add(sum4, sum6);
    sum0 = hn::Add(sum0, sum4);
    
    float result = hn::ReduceSum(d, sum0);
    
    // Scalar tail
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
