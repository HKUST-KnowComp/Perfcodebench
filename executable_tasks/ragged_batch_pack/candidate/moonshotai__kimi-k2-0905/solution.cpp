#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

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
  // one-time capacity reservation
  static thread_local std::vector<uint32_t> packed_buf;
  int total = 0;
  for (int b = 0; b < batch; ++b) total += lengths[static_cast<size_t>(b)];
  if (static_cast<int>(packed_buf.size()) < total) packed_buf.resize(static_cast<size_t>(total));

  uint64_t hash = 1469598103934665603ULL; // hoisted out of iter loop

  for (int iter = 0; iter < iters; ++iter) {
    uint32_t* out = packed_buf.data();
    uint32_t pos  = 0;

    for (int b = 0; b < batch; ++b) {
      int len = lengths[static_cast<size_t>(b)];
      const uint32_t* row = &padded_tokens[static_cast<size_t>(b) * static_cast<size_t>(max_len)];
      std::memcpy(out, row, static_cast<size_t>(len) * sizeof(uint32_t));
      out += len;
      pos += static_cast<uint32_t>(len);
    }

    // checksum: 4-way unrolled
    const uint32_t* tok = packed_buf.data();
    size_t N = static_cast<size_t>(total);
    size_t i = 0;
    uint32_t p = 0;

    for (; i + 3 < N; i += 4) {
      uint32_t p0 = p;
      uint32_t p1 = p + 1;
      uint32_t p2 = p + 2;
      uint32_t p3 = p + 3;
      p += 4;

      hash = mix(hash, tok[i+0]);
      hash = mix(hash, p0);
      hash = mix(hash, tok[i+1]);
      hash = mix(hash, p1);
      hash = mix(hash, tok[i+2]);
      hash = mix(hash, p2);
      hash = mix(hash, tok[i+3]);
      hash = mix(hash, p3);
    }
    for (; i < N; ++i) {
      hash = mix(hash, tok[i]);
      hash = mix(hash, p++);
    }
  }
  return hash;
}