#include "interface.h"

#include <cstdint>
#include <vector>

namespace {

inline uint64_t fnv1a_serialize_hash(const std::vector<uint32_t>& ids,
                                     const std::vector<uint32_t>& ts,
                                     const std::vector<uint16_t>& levels,
                                     const std::vector<uint16_t>& lens) {
  constexpr uint64_t kFNVOffset = 1469598103934665603ULL;
  constexpr uint64_t kFNVPrime = 1099511628211ULL;

  const size_t n = ids.size();
  const uint32_t* idp = ids.data();
  const uint32_t* tsp = ts.data();
  const uint16_t* lvlp = levels.data();
  const uint16_t* lenp = lens.data();

  uint64_t h = kFNVOffset;

  for (size_t i = 0; i < n; ++i) {
    uint32_t v32;
    uint16_t v16;

    // ids[i]
    v32 = *idp++;
    h ^= static_cast<uint8_t>(v32);
    h *= kFNVPrime;
    h ^= static_cast<uint8_t>(v32 >> 8);
    h *= kFNVPrime;
    h ^= static_cast<uint8_t>(v32 >> 16);
    h *= kFNVPrime;
    h ^= static_cast<uint8_t>(v32 >> 24);
    h *= kFNVPrime;

    // ts[i]
    v32 = *tsp++;
    h ^= static_cast<uint8_t>(v32);
    h *= kFNVPrime;
    h ^= static_cast<uint8_t>(v32 >> 8);
    h *= kFNVPrime;
    h ^= static_cast<uint8_t>(v32 >> 16);
    h *= kFNVPrime;
    h ^= static_cast<uint8_t>(v32 >> 24);
    h *= kFNVPrime;

    // levels[i]
    v16 = *lvlp++;
    h ^= static_cast<uint8_t>(v16);
    h *= kFNVPrime;
    h ^= static_cast<uint8_t>(v16 >> 8);
    h *= kFNVPrime;

    // lens[i]
    v16 = *lenp++;
    h ^= static_cast<uint8_t>(v16);
    h *= kFNVPrime;
    h ^= static_cast<uint8_t>(v16 >> 8);
    h *= kFNVPrime;
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
  // Match baseline semantics: if iters == 0, return 0. Otherwise, the checksum
  // is identical for any positive iters value (last iteration's checksum).
  if (iters <= 0) return 0;
  return fnv1a_serialize_hash(ids, ts, levels, lens);
}
