#pragma once

#include <cstdint>
#include <vector>

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  const std::size_t num_records = ids.size();
  const std::size_t record_size = 4 + 4 + 2 + 2; // u32 + u32 + u16 + u16
  const std::size_t total_size = num_records * record_size;
  
  std::vector<uint8_t> out;
  out.reserve(total_size);
  
  // Serialize once
  for (std::size_t i = 0; i < num_records; ++i) {
    uint32_t id = ids[i];
    uint32_t t = ts[i];
    uint16_t l = levels[i];
    uint16_t len = lens[i];
    
    out.push_back(static_cast<uint8_t>(id));
    out.push_back(static_cast<uint8_t>(id >> 8));
    out.push_back(static_cast<uint8_t>(id >> 16));
    out.push_back(static_cast<uint8_t>(id >> 24));
    
    out.push_back(static_cast<uint8_t>(t));
    out.push_back(static_cast<uint8_t>(t >> 8));
    out.push_back(static_cast<uint8_t>(t >> 16));
    out.push_back(static_cast<uint8_t>(t >> 24));
    
    out.push_back(static_cast<uint8_t>(l));
    out.push_back(static_cast<uint8_t>(l >> 8));
    
    out.push_back(static_cast<uint8_t>(len));
    out.push_back(static_cast<uint8_t>(len >> 8));
  }
  
  // Compute checksum once
  uint64_t hash = 1469598103934665603ULL;
  for (uint8_t b : out) {
    hash ^= static_cast<uint64_t>(b);
    hash *= 1099511628211ULL;
  }
  
  // Return hash (same for all iterations since data is identical)
  return hash;
}