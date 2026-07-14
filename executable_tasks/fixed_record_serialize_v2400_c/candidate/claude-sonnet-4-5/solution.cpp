#include "interface.h"

#include <cstring>
#include <vector>

namespace {

struct __attribute__((packed)) Record {
  uint32_t id;
  uint32_t ts;
  uint16_t level;
  uint16_t len;
};

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  const std::size_t n = ids.size();
  const std::size_t bytes_needed = n * sizeof(Record);
  std::vector<uint8_t> out(bytes_needed);
  
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    uint8_t* ptr = out.data();
    
    for (std::size_t i = 0; i < n; ++i) {
      Record rec;
      rec.id = ids[i];
      rec.ts = ts[i];
      rec.level = levels[i];
      rec.len = lens[i];
      std::memcpy(ptr, &rec, sizeof(Record));
      ptr += sizeof(Record);
    }
    
    hash = 1469598103934665603ULL;
    const uint8_t* data = out.data();
    std::size_t i = 0;
    const std::size_t unroll_limit = bytes_needed - (bytes_needed % 4);
    
    for (; i < unroll_limit; i += 4) {
      hash ^= static_cast<uint64_t>(data[i]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(data[i + 1]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(data[i + 2]);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>(data[i + 3]);
      hash *= 1099511628211ULL;
    }
    
    for (; i < bytes_needed; ++i) {
      hash ^= static_cast<uint64_t>(data[i]);
      hash *= 1099511628211ULL;
    }
  }
  
  return hash;
}