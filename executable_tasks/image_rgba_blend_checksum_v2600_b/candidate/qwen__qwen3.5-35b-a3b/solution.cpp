#pragma once

#include <cstdint>
#include <vector>

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
  (void)width;
  (void)height;
  const std::size_t pixels = src.size() / 4u;
  const std::size_t out_size = pixels * 4u;
  std::vector<uint8_t> out(out_size);
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    for (std::size_t i = 0; i < pixels; ++i) {
      const std::size_t idx = i * 4u;
      const uint8_t a = src[idx + 3u];
      const uint8_t s_r = src[idx];
      const uint8_t s_g = src[idx + 1u];
      const uint8_t s_b = src[idx + 2u];
      const uint8_t d_r = dst[idx];
      const uint8_t d_g = dst[idx + 1u];
      const uint8_t d_b = dst[idx + 2u];

      // Blend R
      uint32_t num_r = static_cast<uint32_t>(s_r) * a +
                       static_cast<uint32_t>(d_r) * (255u - a) + 127u;
      out[idx] = static_cast<uint8_t>(num_r / 255u);

      // Blend G
      uint32_t num_g = static_cast<uint32_t>(s_g) * a +
                       static_cast<uint32_t>(d_g) * (255u - a) + 127u;
      out[idx + 1u] = static_cast<uint8_t>(num_g / 255u);

      // Blend B
      uint32_t num_b = static_cast<uint32_t>(s_b) * a +
                       static_cast<uint32_t>(d_b) * (255u - a) + 127u;
      out[idx + 2u] = static_cast<uint8_t>(num_b / 255u);

      // Alpha is always 255
      out[idx + 3u] = 255u;
    }

    // Compute FNV-1a hash
    hash = 1469598103934665603ULL;
    for (std::size_t j = 0; j < out_size; ++j) {
      hash ^= static_cast<uint64_t>(out[j]);
      hash *= 1099511628211ULL;
    }
  }

  return hash;
}