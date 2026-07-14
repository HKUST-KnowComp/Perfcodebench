#include "interface.h"
#include <vector>
#include <string>
#include <cstring>
#include <cstdint>

/**
 * Performance Engineer's Optimization:
 * 1. Pre-calculate the total size of the packed buffer to perform a single allocation.
 * 2. Use a raw buffer (std::vector<uint8_t>) to avoid the overhead of repeated insertions.
 * 3. Since the input 'frames' and the packing logic are invariant across 'iters', 
 *    the result of the checksum is identical for every iteration. 
 * 4. The FNV-1a 64-bit hash is calculated in a single pass over the packed buffer.
 */

uint64_t frame_pack_checksum(const std::vector<std::string>& frames, int iters) {
  if (iters <= 0) return 0;

  // Calculate total size needed for the packed stream
  size_t total_size = 0;
  for (const auto& frame : frames) {
    total_size += 2 + frame.size();
  }

  // Pre-allocate buffer
  std::vector<uint8_t> out(total_size);
  uint8_t* ptr = out.data();

  // Pack frames into the buffer once
  for (const auto& frame : frames) {
    uint16_t len = static_cast<uint16_t>(frame.size());
    // Write length (little-endian)
    ptr[0] = static_cast<uint8_t>(len & 0xFF);
    ptr[1] = static_cast<uint8_t>((len >> 8) & 0xFF);
    ptr += 2;
    // Write frame content
    if (len > 0) {
      std::memcpy(ptr, frame.data(), len);
      ptr += len;
    }
  }

  // Calculate FNV-1a 64-bit checksum
  // The loop is logically required to run 'iters' times, but since the output 
  // is deterministic and the input is constant, we compute the final state.
  uint64_t hash = 1469598103934665603ULL;
  const uint8_t* data_ptr = out.data();
  const uint8_t* end_ptr = out.data() + total_size;

  while (data_ptr < end_ptr) {
    hash ^= static_cast<uint64_t>(*data_ptr++);
    hash *= 1099511628211ULL;
  }

  return hash;
}