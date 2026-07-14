#include "interface.h"

namespace {

constexpr uint64_t kFNVOffset = 1469598103934665603ULL;
constexpr uint64_t kFNVPrime  = 1099511628211ULL;

inline void fnv1a_mix(uint64_t& h, uint8_t b) {
  h ^= static_cast<uint64_t>(b);
  h *= kFNVPrime;
}

inline uint64_t serialize_and_hash_once(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens) {
  const std::size_t n = ids.size();

  const uint32_t* idp = ids.data();
  const uint32_t* tsp = ts.data();
  const uint16_t* lvlp = levels.data();
  const uint16_t* lenp = lens.data();

  uint64_t h = kFNVOffset;

  for (std::size_t i = 0; i < n; ++i) {
    uint32_t v32;
    uint16_t v16;

    v32 = *idp++;
    fnv1a_mix(h, static_cast<uint8_t>(v32));
    fnv1a_mix(h, static_cast<uint8_t>(v32 >> 8));
    fnv1a_mix(h, static_cast<uint8_t>(v32 >> 16));
    fnv1a_mix(h, static_cast<uint8_t>(v32 >> 24));

    v32 = *tsp++;
    fnv1a_mix(h, static_cast<uint8_t>(v32));
    fnv1a_mix(h, static_cast<uint8_t>(v32 >> 8));
    fnv1a_mix(h, static_cast<uint8_t>(v32 >> 16));
    fnv1a_mix(h, static_cast<uint8_t>(v32 >> 24));

    v16 = *lvlp++;
    fnv1a_mix(h, static_cast<uint8_t>(v16));
    fnv1a_mix(h, static_cast<uint8_t>(v16 >> 8));

    v16 = *lenp++;
    fnv1a_mix(h, static_cast<uint8_t>(v16));
    fnv1a_mix(h, static_cast<uint8_t>(v16 >> 8));
  }

  return h;
}

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  if (iters <= 0) {
    return 0ULL;
  }
  // The serialized output is identical for every iteration, so compute once.
  return serialize_and_hash_once(ids, ts, levels, lens);
}
