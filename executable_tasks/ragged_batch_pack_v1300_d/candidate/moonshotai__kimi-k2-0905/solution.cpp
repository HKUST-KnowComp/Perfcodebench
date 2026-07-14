#include "interface.h"
#include <cstdint>
#include <vector>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t ragged_pack_checksum(const std::vector<uint32_t>& padded_tokens,
                               const std::vector<int>& lengths,
                               int batch,
                               int max_len,
                               int iters) {
  const std::size_t maxlen = static_cast<std::size_t>(max_len);
  const std::size_t bsize  = static_cast<std::size_t>(batch);

  // reserve once for full packing
  std::size_t total_tokens = 0;
  for (std::size_t b = 0; b < bsize; ++b) total_tokens += static_cast<std::size_t>(lengths[b]);
  std::vector<uint32_t> packed_tokens;
  std::vector<uint32_t> positions;
  packed_tokens.reserve(total_tokens);
  positions.reserve(total_tokens);

  // row-start offsets for O(1) jump within packed array
  std::vector<std::size_t> rowoff;
  rowoff.reserve(bsize + 1);
  std::size_t off = 0;
  for (std::size_t b = 0; b < bsize; ++b) {
    rowoff.push_back(off);
    off += static_cast<std::size_t>(lengths[b]);
  }
  rowoff.push_back(off); // sentinel

  uint64_t hash = 0;
  for (int it = 0; it < iters; ++it) {
    packed_tokens.clear();
    positions.clear();

    // sequential scan of input array; compute packed order
    for (std::size_t b = 0; b < bsize; ++b) {
      const std::size_t start = rowoff[b];
      const std::size_t rowlen = rowoff[b + 1] - start;
      const uint32_t* src = &padded_tokens[b * maxlen];
      uint32_t* pt = &packed_tokens[start];
      uint32_t* ps = &positions[start];

      // manual unroll by 4
      std::size_t rem = rowlen;
      uint32_t pos = 0;
      for (; rem >= 4; rem -= 4, pos += 4) {
        pt[0] = src[pos];
        ps[0] = pos;
        pt[1] = src[pos + 1];
        ps[1] = pos + 1;
        pt[2] = src[pos + 2];
        ps[2] = pos + 2;
        pt[3] = src[pos + 3];
        ps[3] = pos + 3;
        pt += 4; ps += 4;
      }
      for (; rem; --rem, ++pos) {
        pt[0] = src[pos];
        ps[0] = pos;
        ++pt; ++ps;
      }
    }

    // hash over both packed_tokens and positions
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0, n = packed_tokens.size(); i < n; ++i) {
      hash = mix(hash, packed_tokens[i]);
      hash = mix(hash, positions[i]);
    }
  }
  return hash;
}