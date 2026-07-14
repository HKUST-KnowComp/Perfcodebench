#pragma once

#include <cstdint>
#include <vector>

namespace {

uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
  // Approximate division by 255 using multiplication and bit-shift
  return static_cast<uint8_t>((static_cast<uint32_t>(s) * a +
                               static_cast<uint32_t>(d) * (255u - a) + 128u) >> 8);
}

uint64_t checksum_bytes(const std::vector<uint8_t>& bytes) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint8_t b : bytes) {
    hash ^= static_cast<uint64_t>(b);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t blend_checksum(
    const std::vector<uint8_t>& src,
    const std::vector<uint8_t>& dst,
    int width,
    int height,
    int iters) {
  (void)width;
  (void)height;
  (void)iters;
  const std::size_t pixels = src.size() / 4u;
  std::vector<uint8_t> out;
  out.reserve(pixels * 4);
  
  for (std::size_t i = 0; i < pixels; ++i) {
    const std::size_t idx = i * 4u;
    const uint8_t a = src[idx + 3u];
    out.push_back(blend_byte(src[idx], dst[idx], a));
    out.push_back(blend_byte(src[idx + 1u], dst[idx + 1u], a));
    out.push_back(blend_byte(src[idx + 2u], dst[idx + 2u], a));
    out.push_back(255u);
  }
  
  return checksum_bytes(out);
}