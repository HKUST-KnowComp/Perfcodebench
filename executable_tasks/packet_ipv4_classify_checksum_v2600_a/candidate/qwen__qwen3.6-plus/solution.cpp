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
    const uint8_t* pkt = data;
    for (std::size_t i = 0; i < packet_count; ++i) {
      uint8_t protocol = pkt[9];
      uint8_t ttl = pkt[8];
      uint8_t src0 = pkt[12];
      uint8_t src1 = pkt[13];
      uint16_t dst_port = (static_cast<uint16_t>(pkt[22]) << 8) | pkt[23];

      uint32_t ttl_bucket = ttl >> 6;
      
      uint32_t port_bucket = 3;
      if (protocol == 6 || protocol == 17) {
        if (dst_port < 1024) port_bucket = 0;
        else if (dst_port < 8192) port_bucket = 1;
        else port_bucket = 2;
      }

      uint32_t is_private = (src0 == 10) ||
                            (src0 == 172 && src1 >= 16 && src1 < 32) ||
                            (src0 == 192 && src1 == 168);
      
      uint32_t key = static_cast<uint32_t>(protocol) * 131u + 
                     ttl_bucket * 17u + 
                     (is_private ? 5u : 11u) + 
                     port_bucket * 23u;
                     
      hash ^= key;
      hash *= 1099511628211ULL;
      
      pkt += 64;
    }
  }
  return hash;
}
