#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  
  const size_t n = ids.size();
  const uint32_t* p_ids = ids.data();
  const uint32_t* p_ts = ts.data();
  const uint16_t* p_levels = levels.data();
  const uint16_t* p_lens = lens.data();

  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;

    for (size_t i = 0; i < n; ++i) {
      // Serialize ids[i] (u32)
      uint32_t v_id = p_ids[i];
      hash ^= static_cast<uint64_t>(v_id & 0xFF);
      hash *= prime;
      hash ^= static_cast<uint64_t>((v_id >> 8) & 0xFF);
      hash *= prime;
      hash ^= static_cast<uint64_t>((v_id >> 16) & 0xFF);
      hash *= prime;
      hash ^= static_cast<uint64_t>((v_id >> 24) & 0xFF);
      hash *= prime;

      // Serialize ts[i] (u32)
      uint32_t v_ts = p_ts[i];
      hash ^= static_cast<uint64_t>(v_ts & 0xFF);
      hash *= prime;
      hash ^= static_cast<uint64_t>((v_ts >> 8) & 0xFF);
      hash *= prime;
      hash ^= static_cast<uint64_t>((v_ts >> 16) & 0xFF);
      hash *= prime;
      hash ^= static_cast<uint64_t>((v_ts >> 24) & 0xFF);
      hash *= prime;

      // Serialize levels[i] (u16)
      uint16_t v_lvl = p_levels[i];
      hash ^= static_cast<uint64_t>(v_lvl & 0xFF);
      hash *= prime;
      hash ^= static_cast<uint64_t>((v_lvl >> 8) & 0xFF);
      hash *= prime;

      // Serialize lens[i] (u16)
      uint16_t v_len = p_lens[i];
      hash ^= static_cast<uint64_t>(v_len & 0xFF);
      hash *= prime;
      hash ^= static_cast<uint64_t>((v_len >> 8) & 0xFF);
      hash *= prime;
    }
    final_hash = hash;
  }

  return final_hash;
}