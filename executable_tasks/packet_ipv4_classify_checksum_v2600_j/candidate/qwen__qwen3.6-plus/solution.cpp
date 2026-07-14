#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  const uint8_t* data = packets.data();
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    const uint8_t* p = data;
    const uint8_t* end = data + packet_count * 64;
    
    while (p < end) {
      uint8_t protocol = p[9];
      uint8_t ttl = p[8];
      uint16_t dst_port = (static_cast<uint16_t>(p[22]) << 8) | p[23];
      
      uint32_t ttl_bucket = ttl >> 6;
      
      uint32_t port_bucket = 3;
      if (protocol == 6 || protocol == 17) {
        if (dst_port < 1024) port_bucket = 0;
        else if (dst_port < 8192) port_bucket = 1;
        else port_bucket = 2;
      }
      
      uint32_t src_class = 11;
      uint8_t b0 = p[12];
      uint8_t b1 = p[13];
      if (b0 == 10 || (b0 == 172 && b1 >= 16 && b1 < 32) || (b0 == 192 && b1 == 168)) {
        src_class = 5;
      }
      
      uint32_t key = protocol * 131u + ttl_bucket * 17u + src_class + port_bucket * 23u;
      hash ^= static_cast<uint64_t>(key);
      hash *= 1099511628211ULL;
      
      p += 64;
    }
  }
  return hash;
}