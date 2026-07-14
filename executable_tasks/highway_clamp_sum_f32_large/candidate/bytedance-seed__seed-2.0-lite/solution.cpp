#include "interface.h"
#include <hwy/highway.h>

float clamp_sum(const float* data, std::size_t n) {
    const hwy::ScalableTag<float> d;
    const size_t lanes = hwy::Lanes(d);
    hwy::Vec<decltype(d)> sum_vec = hwy::Zero(d);
    const hwy::Vec<decltype(d)> zero_vec = hwy::Zero(d);

    // Process full SIMD-aligned chunks of input
    const size_t full_chunks = n / lanes;
    for (std::size_t i = 0; i < full_chunks; ++i) {
        const size_t offset = i * lanes;
        hwy::Vec<decltype(d)> current = hwy::LoadU(d, data + offset);
        // Clamp negative values to 0, identical to original conditional logic
        hwy::Vec<decltype(d)> clamped = hwy::Max(d, current, zero_vec);
        sum_vec = hwy::Add(d, sum_vec, clamped);
    }

    // Reduce vector sum to a single scalar value
    float total = hwy::ReduceSum(d, sum_vec);

    // Process any leftover elements that did not fill a full SIMD chunk
    const size_t remainder_start = full_chunks * lanes;
    for (std::size_t i = remainder_start; i < n; ++i) {
        if (data[i] > 0.0f) {
            total += data[i];
        }
    }

    return total;
}