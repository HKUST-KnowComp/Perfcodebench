#include "interface.h"

#include <cstring>
#include <vector>

namespace {

inline uint64_t checksum_bytes(const uint8_t* __restrict data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  const uint8_t* end = data + len;
  while (data != end) {
    hash ^= static_cast<uint64_t>(*data++);
    hash *= 1099511628211ULL;
  }
  return hash;
}

inline void write_len(uint8_t* dst, uint16_t v) {
  dst[0] = static_cast<uint8_t>(v);
  dst[1] = static_cast<uint8_t>(v >> 8);
}

}  // namespace

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  const size_t n = frames.size();
  size_t total = 0;
  for (const std::string& f : frames) total += f.size();
  const size_t packed_size = 2 * n + total;

  std::vector<uint8_t> buffer(packed_size);

  // Build once
  uint8_t* ptr = buffer.data();
  for (const std::string& f : frames) {
    const uint16_t len = static_cast<uint16_t>(f.size());
    write_len(ptr, len);
    ptr += 2;
    std::memcpy(ptr, f.data(), len);
    ptr += len;
  }

  uint64_t hash = 0;
  for (int i = 0; i < iters; ++i) {
    hash = checksum_bytes(buffer.data(), packed_size);
  }
  return hash;
}