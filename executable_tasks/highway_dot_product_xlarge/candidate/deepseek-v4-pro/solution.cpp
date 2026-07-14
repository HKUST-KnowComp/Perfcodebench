#include "interface.h"
#include <hwy/highway.h>

float dot_product(const float* a, const float* b, std::size_t n) {
    using namespace hwy::HWY_NAMESPACE;
    const ScalableTag<float> df;
    const size_t L = Lanes(df);
    size_t i = 0;

    auto sum0 = Zero(df);
    auto sum1 = Zero(df);
    auto sum2 = Zero(df);
    auto sum3 = Zero(df);

    // process 4*L at a time
    for (; i + 4 * L <= n; i += 4 * L) {
        sum0 = MulAdd(LoadU(df, a + i), LoadU(df, b + i), sum0);
        sum1 = MulAdd(LoadU(df, a + i + L), LoadU(df, b + i + L), sum1);
        sum2 = MulAdd(LoadU(df, a + i + 2 * L), LoadU(df, b + i + 2 * L), sum2);
        sum3 = MulAdd(LoadU(df, a + i + 3 * L), LoadU(df, b + i + 3 * L), sum3);
    }

    sum0 = Add(sum0, sum1);
    sum2 = Add(sum2, sum3);
    sum0 = Add(sum0, sum2);
    float result = ReduceAdd(df, sum0);

    for (; i < n; ++i) {
        result += a[i] * b[i];
    }
    return result;
}
