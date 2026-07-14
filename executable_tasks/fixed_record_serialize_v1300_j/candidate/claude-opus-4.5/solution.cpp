#include "interface.h"

#include <cstdint>
#include <cstring>
#include <vector>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  
  const std::size_t n = ids.size();
  uint64_t hash = 0;
  
  // Compute checksum directly from input arrays without serializing
  // Each record is: id(4 bytes LE) + ts(4 bytes LE) + level(2 bytes LE) + len(2 bytes LE) = 12 bytes
  // FNV-1a processes bytes in order
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    for (std::size_t i = 0; i < n; ++i) {
      // Process id (4 bytes, little-endian)
      uint32_t id = ids[i];
      hash ^= static_cast<uint64_t>(id & 0xFF);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>((id >> 8) & 0xFF);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>((id >> 16) & 0xFF);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>((id >> 24) & 0xFF);
      hash *= 1099511628211ULL;
      
      // Process ts (4 bytes, little-endian)
      uint32_t t = ts[i];
      hash ^= static_cast<uint64_t>(t & 0xFF);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>((t >> 8) & 0xFF);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>((t >> 16) & 0xFF);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>((t >> 24) & 0xFF);
      hash *= 1099511628211ULL;
      
      // Process level (2 bytes, little-endian)
      uint16_t level = levels[i];
      hash ^= static_cast<uint64_t>(level & 0xFF);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>((level >> 8) & 0xFF);
      hash *= 1099511628211ULL;
      
      // Process len (2 bytes, little-endian)
      uint16_t len = lens[i];
      hash ^= static_cast<uint64_t>(len & 0xFF);
      hash *= 1099511628211ULL;
      hash ^= static_cast<uint64_t>((len >> 8) & 0xFF);
      hash *= 1099511628211ULL;
    }
  }
  
  return hash;
}