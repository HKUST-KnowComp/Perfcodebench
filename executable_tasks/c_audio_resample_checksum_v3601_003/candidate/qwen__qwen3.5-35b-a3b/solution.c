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
  if (input_len <= 0 || output_len <= 0 || iters <= 0) {
    return 0;
  }
  
  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;
  
  // Fixed-point arithmetic: 16 fractional bits
  // Precompute increment: (input_len - 1) / (output_len - 1) in fixed-point
  int32_t increment = ((int32_t)(input_len - 1) << 16) / (output_len - 1);
  int32_t pos = 0;
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    for (int i = 0; i < output_len; ++i) {
      int left = pos >> 16;
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;
      
      int frac = pos & 0xFFFF;
      int frac_q = 65536 - frac;
      
      // Linear interpolation using fixed-point
      int32_t sample = (int32_t)src[left] * frac_q + (int32_t)src[right] * frac;
      
      // Round to nearest integer (add half before shift)
      sample = (sample + 32768) >> 16;
      
      // Clamp to int16 range
      if (sample > 32767) sample = 32767;
      if (sample < -32768) sample = -32768;
      
      out[i] = (int16_t)sample;
      
      pos += increment;
    }
    hash = checksum_i16(out, (size_t)output_len);
  }
  
  free(out);
  return hash;
}