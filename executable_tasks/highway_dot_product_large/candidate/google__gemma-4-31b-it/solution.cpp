#include "interface.h"
#include <hwy/highway.h>

namespace hw = hwy;

// Define the target for Highway SIMD
struct DotProductTarget : hw::ScalarTarget {};

// The vectorized kernel
float ComputeDotProduct(const float* a, const float* b, std::size_t n) {
    const hw::HWY_FLOAT f32 = hw::Constant(hw::HWY_FLOAT, 0.0f);
    hw::HWY_FLOAT sum0 = f32;
    hw::HWY_FLOAT sum1 = f32;
    hw::HWY_FLOAT sum2 = f32;
    hw::HWY_FLOAT sum3 = f32;

    const std::size_t lane_size = hw::Lanes(hw::HWY_FLOAT);
    const std::size_t block_size = lane_size * 4;
    std::size_t i = 0;

    // Unroll by 4 to improve ILP and hide latency of fma
    for (; i + block_size <= n; i += block_size) {
        sum0 = hw::Add(sum0, hw::Mul(hw::Load(a + i), hw::Load(b + i)));
        sum1 = hw::Add(sum1, hw::Mul(hw::Load(a + i + lane_size), hw::Load(b + i + lane_size)));
        sum2 = hw::Add(sum2, hw::Mul(hw::Load(a + i + 2 * lane_size), hw::Load(b + i + 2 * lane_size)));
        sum3 = hw::Add(sum3, hw::Mul(hw::Load(a + i + 3 * lane_size), hw::Load(b + i + 3 * lane_size)));
    }

    // Handle remaining blocks of lane_size
    for (; i + lane_size <= n; i += lane_size) {
        sum0 = hw::Add(sum0, hw::Mul(hw::Load(a + i), hw::Load(b + i)));
    }

    // Combine the 4 accumulators
    hw::HWY_FLOAT total_sum = hw::Add(hw::Add(sum0, sum1), hw::Add(sum2, sum3));

    // Horizontal reduction of the SIMD register
    float final_sum = hw::ReduceSum(total_sum);

    // Handle remaining elements (tail)
    for (; i < n; ++i) {
        final_sum += a[i] * b[i];
    }

    return final_sum;
}

// Highway requires the kernel to be defined in a specific way for dispatch
// However, for a simple dot product, we can use the target directly if we know the architecture
// or use the generic dispatch mechanism. Here we use the standard Highway pattern.

float dot_product(const float* a, const float* b, std::size_t n) {
    if (n == 0) return 0.0f;
    // We use the target-specific implementation
    return ComputeDotProduct(a, b, n);
}
