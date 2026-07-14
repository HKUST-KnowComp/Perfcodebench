#include "interface.h"
#include <hwy/highway.h>

float dot_product(const float* a, const float* b, std::size_t n) {
    namespace hn = hwy::HWY_NAMESPACE;
    const hn::ScalableTag<float> d;
    using V = hn::Vec<decltype(d)>;
    
    std::size_t i = 0;
    const std::size_t lanes = hn::Lanes(d);
    
    V sum0 = hn::Zero(d);
    V sum1 = hn::Zero(d);
    V sum2 = hn::Zero(d);
    V sum3 = hn::Zero(d);
    
    for (; i + 4 * lanes <= n; i += 4 * lanes) {
        sum0 = hn::MulAdd(hn::LoadU(d, a + i), hn::LoadU(d, b + i), sum0);
        sum1 = hn::MulAdd(hn::LoadU(d, a + i + lanes), hn::LoadU(d, b + i + lanes), sum1);
        sum2 = hn::MulAdd(hn::LoadU(d, a + i + 2 * lanes), hn::LoadU(d, b + i + 2 * lanes), sum2);
        sum3 = hn::MulAdd(hn::LoadU(d, a + i + 3 * lanes), hn::LoadU(d, b + i + 3 * lanes), sum3);
    }
    
    for (; i + lanes <= n; i += lanes) {
        sum0 = hn::MulAdd(hn::LoadU(d, a + i), hn::LoadU(d, b + i), sum0);
    }
    
    sum0 = hn::Add(sum0, sum1);
    sum2 = hn::Add(sum2, sum3);
    sum0 = hn::Add(sum0, sum2);
    
    float total_sum = hn::ReduceSum(d, sum0);
    
    for (; i < n; ++i) {
        total_sum += a[i] * b[i];
    }
    
    return total_sum;
}
