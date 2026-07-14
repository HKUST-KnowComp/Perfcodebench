#include "interface.h"
#include <vector>
#include <cstddef>
#include <cstdint>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

} // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens,
                              const std::vector<int>& lengths,
                              int batch,
                              int max_len,
                              int iters) {
  thread_local std::vector<uint32_t> packed_buf;
  packed_buf.clear();
  packed_buf.reserve(static_cast<std::size_t>(batch) * static_cast<std::size_t>(max_len));

  const std::size_t bsz = static_cast<std::size_t>(batch);
  const std::size_t mxl = static_cast<std::size_t>(max_len);

  // build packed sequence once
  for (std::size_t b = 0; b < bsz; ++b) {
    const std::size_t len = static_cast<std::size_t>(lengths[b]);
    const std::size_t off = b * mxl;
    for (std::size_t i = 0; i < len; ++i) {
      packed_buf.push_back(padded_tokens[off + i]);
    }
  }

  const std::size_t N = packed_buf.size();
  uint64_t hash = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = hash;
    std::size_t i = 0;
    // unroll 4x
    for (; i + 3 < N; i += 4) {
      h = mix(h, packed_buf[i]);
      h = mix(h, static_cast<uint64_t>(i));
      h = mix(h, packed_buf[i+1]);
      h = mix(h, static_cast<uint64_t>(i+1));
      h = mix(h, packed_tokens[i+2]);
      h = mix(h, static_cast<uint64_t>(i+2));
      h = mix(h, packed_tokens[i+3]);
      h = mix(h, static_cast<uint64_t>(i+3));
    }
    // tail
    for (; i < N; ++i) {
      h = mix(h, packed_buf[i]);
      h = mix(h, static_cast<uint64_t>(i));
    }
    hash = h;
  }
  return hash;
}