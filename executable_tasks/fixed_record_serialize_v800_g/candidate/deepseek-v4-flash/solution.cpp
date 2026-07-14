#include "interface.h"

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  constexpr uint64_t kOffset = 1469598103934665603ULL;
  constexpr uint64_t kPrime  = 1099511628211ULL;

  uint64_t hash = 0;
  const uint64_t n = ids.size();
  const uint32_t* ids_data = ids.data();
  const uint32_t* ts_data = ts.data();
  const uint16_t* levels_data = levels.data();
  const uint16_t* lens_data = lens.data();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = kOffset;
    for (uint64_t i = 0; i < n; ++i) {
      uint32_t id = ids_data[i];
      h ^= static_cast<uint64_t>(id & 0xFF);
      h *= kPrime;
      h ^= static_cast<uint64_t>((id >> 8) & 0xFF);
      h *= kPrime;
      h ^= static_cast<uint64_t>((id >> 16) & 0xFF);
      h *= kPrime;
      h ^= static_cast<uint64_t>((id >> 24) & 0xFF);
      h *= kPrime;

      uint32_t t = ts_data[i];
      h ^= static_cast<uint64_t>(t & 0xFF);
      h *= kPrime;
      h ^= static_cast<uint64_t>((t >> 8) & 0xFF);
      h *= kPrime;
      h ^= static_cast<uint64_t>((t >> 16) & 0xFF);
      h *= kPrime;
      h ^= static_cast<uint64_t>((t >> 24) & 0xFF);
      h *= kPrime;

      uint16_t level = levels_data[i];
      h ^= static_cast<uint64_t>(level & 0xFF);
      h *= kPrime;
      h ^= static_cast<uint64_t>((level >> 8) & 0xFF);
      h *= kPrime;

      uint16_t len = lens_data[i];
      h ^= static_cast<uint64_t>(len & 0xFF);
      h *= kPrime;
      h ^= static_cast<uint64_t>((len >> 8) & 0xFF);
      h *= kPrime;
    }
    hash = h;
  }
  return hash;
}
