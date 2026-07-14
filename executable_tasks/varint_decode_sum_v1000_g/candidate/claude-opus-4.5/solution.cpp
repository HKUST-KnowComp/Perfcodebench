#include "interface.h"

#include <cstdint>
#include <vector>

uint64_t varint_sum(const std::vector<uint8_t>& encoded, int iters) {
  uint64_t sum = 0;
  const uint8_t* const data = encoded.data();
  const std::size_t size = encoded.size();
  
  for (int iter = 0; iter < iters; ++iter) {
    sum = 0;
    const uint8_t* ptr = data;
    const uint8_t* const end = data + size;
    
    // Process with prefetching for large streams
    while (ptr < end) {
      // Prefetch ahead for better cache performance
      if (end - ptr > 64) {
        __builtin_prefetch(ptr + 64, 0, 0);
      }
      
      uint8_t byte = *ptr++;
      
      // Fast path: single byte varint (most common case, values 0-127)
      if ((byte & 0x80) == 0) {
        sum += byte;
        continue;
      }
      
      uint64_t value = byte & 0x7F;
      
      // Second byte
      byte = *ptr++;
      if ((byte & 0x80) == 0) {
        value |= static_cast<uint64_t>(byte) << 7;
        sum += value;
        continue;
      }
      value |= static_cast<uint64_t>(byte & 0x7F) << 7;
      
      // Third byte
      byte = *ptr++;
      if ((byte & 0x80) == 0) {
        value |= static_cast<uint64_t>(byte) << 14;
        sum += value;
        continue;
      }
      value |= static_cast<uint64_t>(byte & 0x7F) << 14;
      
      // Fourth byte
      byte = *ptr++;
      if ((byte & 0x80) == 0) {
        value |= static_cast<uint64_t>(byte) << 21;
        sum += value;
        continue;
      }
      value |= static_cast<uint64_t>(byte & 0x7F) << 21;
      
      // Fifth byte (max for 32-bit varint)
      byte = *ptr++;
      value |= static_cast<uint64_t>(byte & 0x7F) << 28;
      
      // Handle potential 64-bit varints (up to 10 bytes)
      if (byte & 0x80) {
        byte = *ptr++;
        if ((byte & 0x80) == 0) {
          value |= static_cast<uint64_t>(byte) << 35;
          sum += value;
          continue;
        }
        value |= static_cast<uint64_t>(byte & 0x7F) << 35;
        
        byte = *ptr++;
        if ((byte & 0x80) == 0) {
          value |= static_cast<uint64_t>(byte) << 42;
          sum += value;
          continue;
        }
        value |= static_cast<uint64_t>(byte & 0x7F) << 42;
        
        byte = *ptr++;
        if ((byte & 0x80) == 0) {
          value |= static_cast<uint64_t>(byte) << 49;
          sum += value;
          continue;
        }
        value |= static_cast<uint64_t>(byte & 0x7F) << 49;
        
        byte = *ptr++;
        if ((byte & 0x80) == 0) {
          value |= static_cast<uint64_t>(byte) << 56;
          sum += value;
          continue;
        }
        value |= static_cast<uint64_t>(byte & 0x7F) << 56;
        
        byte = *ptr++;
        value |= static_cast<uint64_t>(byte & 0x01) << 63;
      }
      
      sum += value;
    }
  }
  return sum;
}