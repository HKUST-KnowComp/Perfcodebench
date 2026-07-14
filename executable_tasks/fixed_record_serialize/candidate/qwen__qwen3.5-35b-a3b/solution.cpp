#pragma once

#include <cstdint>
#include <vector>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  const std::size_t record_size = 12; // 4+4+2+2 bytes
  const std::size_t total_size = ids.size() * record_size;
  std::vector<uint8_t> out;
  out.reserve(total_size);
  
  uint64_t hash = 1469598103934665603ULL;
  
  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    out.resize(total_size);
    uint8_t* ptr = out.data();
    
    for (std::size_t i = 0; i < ids.size(); ++i) {
      // Write u32 (little-endian)
      *ptr++ = static_cast<uint8_t>(ids[i]);
      *ptr++ = static_cast<uint8_t>(ids[i] >> 8);
      *ptr++ = static_cast<uint8_t>(ids[i] >> 16);
      *ptr++ = static_cast<uint8_t>(ids[i] >> 24);
      
      *ptr++ = static_cast<uint8_t>(ts[i]);
      *ptr++ = static_cast<uint8_t>(ts[i] >> 8);
      *ptr++ = static_cast<uint8_t>(ts[i] >> 16);
      *ptr++ = static_cast<uint8_t>(ts[i] >> 24);
      
      // Write u16 (little-endian)
      *ptr++ = static_cast<uint8_t>(levels[i]);
      *ptr++ = static_cast<uint8_t>(levels[i] >> 8);
      
      *ptr++ = static_cast<uint8_t>(lens[i]);
      *ptr++ = static_cast<uint8_t>(lens[i] >> 8);
    }
    
    // Compute checksum only on last iteration
    if (iter == iters - 1) {
      for (uint8_t b : out) {
        hash ^= static_cast<uint64_t>(b);
        hash *= 1099511628211ULL;
      }
    }
  }
  
  return hash;
}