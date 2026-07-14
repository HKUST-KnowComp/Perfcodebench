#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  uint64_t hash = 0;
  const uint8_t* base = packets.data();
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    const uint8_t* p = base;
    for (std::size_t i = 0; i < packet_count; ++i, p += 64) {
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
      
      uint8_t src0 = p[12];
      uint8_t src1 = p[13];
      bool is_priv = (src0 == 10) || 
                     (src0 == 172 && src1 >= 16 && src1 < 32) || 
                     (src0 == 192 && src1 == 168);
                     
      uint32_t key = static_cast<uint32_t>(protocol) * 131u + 
                     ttl_bucket * 17u + 
                     (is_priv ? 5u : 11u) + 
                     port_bucket * 23u;
                     
      hash ^= static_cast<uint64_t>(key);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}