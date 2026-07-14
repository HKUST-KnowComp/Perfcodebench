#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

namespace {

// FNV-1a constants
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

} // namespace

uint64_t serialize_records_checksum(
    const std::vector<uint32_t>& ids,
    const std::vector<uint32_t>& ts,
    const std::vector<uint16_t>& levels,
    const std::vector<uint16_t>& lens,
    int iters) {
  
  const size_t n = ids.size();
  if (n == 0) return 0;

  // Each record is 4 + 4 + 2 + 2 = 12 bytes
  const size_t record_size = 12;
  const size_t total_bytes = n * record_size;
  
  // Pre-allocate buffer to avoid reallocations
  std::vector<uint8_t> buffer(total_bytes);
  uint8_t* ptr = buffer.data();

  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // We need to calculate the checksum of the serialized buffer.
    // To optimize, we can perform the serialization and checksumming in one pass,
    // but the requirement is to return the checksum of the 'serialized bytes'.
    // The baseline serializes everything then hashes. 
    // We will replicate the exact byte order (little-endian) and then hash.
    
    for (size_t i = 0; i < n; ++i) {
      // Write ID (u32)
      uint32_t id = ids[i];
      *ptr++ = static_cast<uint8_t>(id);
      *ptr++ = static_cast<uint8_t>(id >> 8);
      *ptr++ = static_cast<uint8_t>(id >> 16);
      *ptr++ = static_cast<uint8_t>(id >> 24);

      // Write TS (u32)
      uint32_t t = ts[i];
      *ptr++ = static_cast<uint8_t>(t);
      *ptr++ = static_cast<uint8_t>(t >> 8);
      *ptr++ = static_cast<uint8_t>(t >> 16);
      *ptr++ = static_cast<uint8_t>(t >> 24);

      // Write Level (u16)
      uint16_t l = levels[i];
      *ptr++ = static_cast<uint8_t>(l);
      *ptr++ = static_cast<uint8_t>(l >> 8);

      // Write Lens (u16)
      uint16_t ln = lens[i];
      *ptr++ = static_cast<uint8_t>(ln);
      *ptr++ = static_cast<uint8_t>(ln >> 8);
    }

    // Calculate FNV-1a checksum
    uint64_t hash = FNV_OFFSET_BASIS;
    const uint8_t* hash_ptr = buffer.data();
    for (size_t i = 0; i < total_bytes; ++i) {
      hash ^= static_cast<uint64_t>(hash_ptr[i]);
      hash *= FNV_PRIME;
    }
    final_hash = hash;
  }

  return final_hash;
}
