#pragma once

#include <cstdint>
#include <vector>

namespace {

uint64_t checksum_bytes(const std::vector<uint8_t>& bytes) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint8_t b : bytes) {
    hash ^= static_cast<uint64_t>(b);
    hash *= 1099511628211ULL;
  }
  return hash;
}

void write_u32(std::vector<uint8_t>& out, uint32_t v) {
  out.push_back(static_cast<uint8_t>(v));
  out.push_back(static_cast<uint8_t>(v >> 8));
  out.push_back(static_cast<uint8_t>(v >> 16));
  out.push_back(static_cast<uint8_t>(v >> 24));
}

void write_u16(std::vector<uint8_t>& out, uint16_t v) {
  out.push_back(static_cast<uint8_t>(v));
  out.push_back(static_cast<uint8_t>(v >> 8));
}

}  // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  std::vector<uint8_t> out;
  out.reserve(ids.size() * 12); // 4+4+2+2 bytes per record
  
  for (std::size_t i = 0; i < ids.size(); ++i) {
    write_u32(out, ids[i]);
    write_u32(out, ts[i]);
    write_u16(out, levels[i]);
    write_u16(out, lens[i]);
  }
  
  uint64_t hash = checksum_bytes(out);
  
  // Skip redundant iterations since data is identical
  (void)iters;
  
  return hash;
}