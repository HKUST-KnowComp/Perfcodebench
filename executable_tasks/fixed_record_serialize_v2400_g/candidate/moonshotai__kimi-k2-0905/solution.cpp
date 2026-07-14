#include "interface.h"
#include <vector>
#include <cstring>

namespace {

inline void store_le32(uint8_t* p, uint32_t v) noexcept {
  p[0] = static_cast<uint8_t>(v);
  p[1] = static_cast<uint8_t>(v >> 8);
  p[2] = static_cast<uint8_t>(v >> 16);
  p[3] = static_cast<uint8_t>(v >> 24);
}

inline void store_le16(uint8_t* p, uint16_t v) noexcept {
  p[0] = static_cast<uint8_t>(v);
  p[1] = static_cast<uint8_t>(v >> 8);
}

} // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  const size_t n = ids.size();
  const size_t record_size = 4 + 4 + 2 + 2; // 12 bytes
  const size_t total_size = n * record_size;
  std::vector<uint8_t> buffer(total_size);

  uint64_t hash = 1469598103934665603ULL;
  constexpr uint64_t prime = 1099511628211ULL;

  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* out = buffer.data();
    hash = 1469598103934665603ULL;

    for (size_t i = 0; i < n; ++i) {
      const uint32_t id = ids[i];
      const uint32_t t = ts[i];
      const uint16_t lvl = levels[i];
      const uint16_t ln = lens[i];

      store_le32(out, id);
      hash ^= static_cast<uint64_t>(out[0]); hash *= prime;
      hash ^= static_cast<uint64_t>(out[1]); hash *= prime;
      hash ^= static_cast<uint64_t>(out[2]); hash *= prime;
      hash ^= static_cast<uint64_t>(out[3]); hash *= prime;
      out += 4;

      store_le32(out, t);
      hash ^= static_cast<uint64_t>(out[0]); hash *= prime;
      hash ^= static_cast<uint64_t>(out[1]); hash *= prime;
      hash ^= static_cast<uint64_t>(out[2]); hash *= prime;
      hash ^= static_cast<uint64_t>(out[3]); hash *= prime;
      out += 4;

      store_le16(out, lvl);
      hash ^= static_cast<uint64_t>(out[0]); hash *= prime;
      hash ^= static_cast<uint64_t>(out[1]); hash *= prime;
      out += 2;

      store_le16(out, ln);
      hash ^= static_cast<uint64_t>(out[0]); hash *= prime;
      hash ^= static_cast<uint64_t>(out[1]); hash *= prime;
      out += 2;
    }
  }
  return hash;
}