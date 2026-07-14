#pragma once

#include <cstdint>
#include <vector>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  const std::size_t n = ids.size();
  const std::size_t bytes_per_record = 12; // 4+4+2+2
  const std::size_t total_bytes = n * bytes_per_record;
  
  std::vector<uint8_t> out;
  out.reserve(total_bytes);
  
  uint64_t hash = 1469598103934665603ULL;
  
  for (int iter = 0; iter < iters; ++iter) {
    out.clear();
    out.reserve(total_bytes);
    
    for (std::size_t i = 0; i < n; ++i) {
      uint32_t id = ids[i];
      uint32_t t = ts[i];
      uint16_t l = levels[i];
      uint16_t len = lens[i];
      
      // Write u32 id (little-endian)
      out.push_back(static_cast<uint8_t>(id));
      out.push_back(static_cast<uint8_t>(id >> 8));
      out.push_back(static_cast<uint8_t>(id >> 16));
      out.push_back(static_cast<uint8_t>(id >> 24));
      
      // Write u32 ts (little-endian)
      out.push_back(static_cast<uint8_t>(t));
      out.push_back(static_cast<uint8_t>(t >> 8));
      out.push_back(static_cast<uint8_t>(t >> 16));
      out.push_back(static_cast<uint8_t>(t >> 24));
      
      // Write u16 level (little-endian)
      out.push_back(static_cast<uint8_t>(l));
      out.push_back(static_cast<uint8_t>(l >> 8));
      
      // Write u16 len (little-endian)
      out.push_back(static_cast<uint8_t>(len));
      out.push_back(static_cast<uint8_t>(len >> 8));
      
      // Incremental checksum
      for (std::size_t j = out.size() - 12; j < out.size(); ++j) {
        hash ^= static_cast<uint64_t>(out[j]);
        hash *= 1099511628211ULL;
      }
    }
  }
  
  return hash;
}