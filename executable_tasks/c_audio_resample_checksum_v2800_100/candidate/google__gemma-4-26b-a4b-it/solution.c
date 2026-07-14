#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static uint64_t checksum_i16(const int16_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < n; ++i) {
    hash ^= (uint16_t)data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
  if (output_len <= 0) return 0;
  
  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;

  uint64_t hash = 0;

  if (output_len == 1) {
    for (int iter = 0; iter < iters; ++iter) {
      out[0] = src[0];
      hash = checksum_i16(out, (size_t)output_len);
    }
  } else {
    // Use 64-bit fixed point for high precision interpolation
    // pos = i * (input_len - 1) / (output_len - 1)
    // We use a scale of 2^32 to represent the fractional part
    const uint64_t scale = 1ULL << 32;
    const uint64_t step_num = (uint64_t)(input_len - 1) << 32;
    const uint64_t step_den = (uint64_t)(output_len - 1);

    for (int iter = 0; iter < iters; ++iter) {
      for (int i = 0; i < output_len; ++i) {
        // pos_fixed = i * (input_len - 1) / (output_len - 1) * 2^32
        // To avoid overflow during multiplication, we use __int128 or careful ordering
        // However, (i * step_num) / step_den fits in uint64_t if input_len is reasonable.
        // Given typical audio lengths, we use uint64_t with care.
        uint64_t pos_fixed = ((uint64_t)i * step_num) / step_den;
        
        int left = (int)(pos_fixed >> 32);
        uint32_t frac_fixed = (uint32_t)(pos_fixed & 0xFFFFFFFFULL);
        
        int right = left + 1;
        if (right >= input_len) right = input_len - 1;

        // Linear interpolation: sample = src[left] + frac * (src[right] - src[left])
        // Using fixed point: (src[left] << 32) + (frac_fixed * (src[right] - src[left]))
        int64_t s_left = src[left];
        int64_t s_right = src[right];
        int64_t diff = s_right - s_left;
        
        // We need to simulate the double precision rounding: (sample + 0.5) if positive
        // sample = s_left + (diff * frac_fixed) / 2^32
        // To include the 0.5 rounding: 
        // result = s_left + (diff * frac_fixed + (diff >= 0 ? 0x80000000 : -0x80000000)) / 2^32
        // Wait, the original code uses: sample = (double)src[left] * (1.0 - frac) + (double)src[right] * frac;
        // which is sample = src[left] + frac * (src[right] - src[left])
        // and then rounding: if (sample >= 0) sample + 0.5 else sample - 0.5
        
        int64_t term2 = (diff * (int64_t)frac_fixed);
        // term2 is in 2^32 scale. To get the integer part with 0.5 rounding:
        // We want floor(val + 0.5) for positive, ceil(val - 0.5) for negative.
        // This is equivalent to rounding to nearest integer.
        
        // Let's use the exact logic: 
        // sample_scaled = (s_left << 32) + (diff * frac_fixed)
        // We want to round (sample_scaled / 2^32) to nearest integer.
        
        int64_t sample_scaled = (s_left << 32) + term2;
        int64_t rounded_sample;
        if (sample_scaled >= 0) {
            rounded_sample = (sample_scaled + 0x80000000LL) >> 32;
        } else {
            rounded_sample = (sample_scaled - 0x80000000LL) >> 32;
        }
        out[i] = (int16_t)rounded_sample;
      }
      hash = checksum_i16(out, (size_t)output_len);
    }
  }

  free(out);
  return hash;
}