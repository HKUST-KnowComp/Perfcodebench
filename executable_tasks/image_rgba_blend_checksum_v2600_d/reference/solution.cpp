#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint8_t blend_byte(uint8_t s, uint8_t d, uint8_t a) {
  return static_cast<uint8_t>((static_cast<uint32_t>(s) * a +
                               static_cast<uint32_t>(d) * (255u - a) + 127u) / 255u);
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
  const std::size_t pixels = static_cast<std::size_t>(width) * static_cast<std::size_t>(height);
  std::vector<uint8_t> out(pixels * 4u);
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    const uint8_t* src_ptr = src.data();
    const uint8_t* dst_ptr = dst.data();
    uint8_t* out_ptr = out.data();
    for (std::size_t i = 0; i < pixels; ++i) {
      const uint8_t a = src_ptr[3];
      out_ptr[0] = blend_byte(src_ptr[0], dst_ptr[0], a);
      out_ptr[1] = blend_byte(src_ptr[1], dst_ptr[1], a);
      out_ptr[2] = blend_byte(src_ptr[2], dst_ptr[2], a);
      out_ptr[3] = 255u;
      src_ptr += 4;
      dst_ptr += 4;
      out_ptr += 4;
    }
    hash = checksum_bytes(out);
  }
  return hash;
}
