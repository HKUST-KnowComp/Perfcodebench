#include "interface.h"
#include <vector>
#include <cstdint>
#include <cstring>

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
  const size_t total_size = n * record_size;
  
  // Pre-allocate buffer to avoid reallocations
  std::vector<uint8_t> buffer(total_size);
  uint8_t* ptr = buffer.data();

  uint64_t final_hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    // We need to calculate the checksum of the buffer after it is fully populated.
    // To optimize, we can perform the serialization and checksumming in one pass,
    // but the requirement is to return the checksum of the serialized bytes.
    // The baseline calculates checksum on the final buffer.
    
    uint8_t* curr = ptr;
    for (size_t i = 0; i < n; ++i) {
      uint32_t id = ids[i];
      uint32_t t = ts[i];
      uint16_t l = levels[i];
      uint16_t ln = lens[i];

      // Manual unrolled writes for speed (Little Endian as per baseline)
      curr[0] = static_cast<uint8_t>(id);
      curr[1] = static_cast<uint8_t>(id >> 8);
      curr[2] = static_cast<uint8_t>(id >> 16);
      curr[3] = static_cast<uint8_t>(id >> 24);

      curr[4] = static_cast<uint8_t>(t);
      curr[5] = static_cast<uint8_t>(t >> 8);
      curr[6] = static_cast<uint8_t>(t >> 16);
      curr[7] = static_cast<uint8_t>(t >> 24);

      curr[8] = static_cast<uint8_t>(l);
      curr[9] = static_cast<uint8_t>(l >> 8);

      curr[10] = static_cast<uint8_t>(ln);
      curr[11] = static_cast<uint8_t>(ln >> 8);

      curr += record_size;
    }

    // FNV-1a checksum
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < total_size; ++i) {
      hash ^= static_cast<uint64_t>(buffer[i]);
      hash *= 1099511628211ULL;
    }
    final_hash = hash;
  }

  return final_hash;
}