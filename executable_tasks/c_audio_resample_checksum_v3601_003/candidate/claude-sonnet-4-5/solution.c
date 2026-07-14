#include "interface.h"

#include <stdint.h>
#include <stdlib.h>

static uint64_t checksum_i16(const int16_t* data, size_t n) {
  uint64_t hash = 1469598103934665603ULL;
  size_t i = 0;
  
  // Process 4 elements at a time
  for (; i + 3 < n; i += 4) {
    uint64_t h0 = hash;
    uint64_t h1 = hash;
    uint64_t h2 = hash;
    uint64_t h3 = hash;
    
    h0 ^= (uint16_t)data[i];
    h0 *= 1099511628211ULL;
    
    h1 ^= (uint16_t)data[i+1];
    h1 *= 1099511628211ULL;
    
    h2 ^= (uint16_t)data[i+2];
    h2 *= 1099511628211ULL;
    
    h3 ^= (uint16_t)data[i+3];
    h3 *= 1099511628211ULL;
    
    hash = h0;
    hash ^= (uint16_t)data[i+1];
    hash *= 1099511628211ULL;
    hash ^= (uint16_t)data[i+2];
    hash *= 1099511628211ULL;
    hash ^= (uint16_t)data[i+3];
    hash *= 1099511628211ULL;
  }
  
  // Handle remaining elements
  for (; i < n; ++i) {
    hash ^= (uint16_t)data[i];
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t run(const int16_t* src, int input_len, int output_len, int iters) {
  uint64_t hash = 0;
  
  int16_t* out = (int16_t*)malloc((size_t)output_len * sizeof(int16_t));
  if (!out) return 0;
  
  // Precompute scale factor
  double scale = (double)(input_len - 1) / (double)(output_len - 1);
  
  for (int iter = 0; iter < iters; ++iter) {
    for (int i = 0; i < output_len; ++i) {
      double pos = (double)i * scale;
      int left = (int)pos;
      int right = left + 1;
      if (right >= input_len) right = input_len - 1;
      
      // Use fixed-point arithmetic for interpolation
      // Scale fraction to 16-bit fixed point
      int frac_fixed = (int)((pos - (double)left) * 65536.0);
      int inv_frac = 65536 - frac_fixed;
      
      int32_t sample = ((int32_t)src[left] * inv_frac + (int32_t)src[right] * frac_fixed + 32768) >> 16;
      out[i] = (int16_t)sample;
    }
    hash = checksum_i16(out, (size_t)output_len);
  }
  
  free(out);
  return hash;
}