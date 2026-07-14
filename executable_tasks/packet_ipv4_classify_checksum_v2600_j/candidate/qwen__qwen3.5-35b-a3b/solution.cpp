#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  uint64_t hash = 0;
  const uint8_t* data = packets.data();
  const std::size_t kPacketSize = 64;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < packet_count; ++i) {
      const uint8_t* pkt = data + i * kPacketSize;
      
      // Extract fields directly without memcpy
      const uint8_t protocol = pkt[9];
      const uint8_t ttl = pkt[8];
      const uint8_t src0 = pkt[12];
      const uint8_t src1 = pkt[13];
      const uint16_t dst_port = static_cast<uint16_t>(pkt[22]) << 8 | pkt[23];
      
      // TTL bucket: ttl >> 6
      const uint32_t ttl_bucket = ttl >> 6;
      
      // Port bucket
      uint32_t port_bucket;
      if (protocol == 6 || protocol == 17) {
        if (dst_port < 1024) {
          port_bucket = 0;
        } else if (dst_port < 8192) {
          port_bucket = 1;
        } else {
          port_bucket = 2;
        }
      } else {
        port_bucket = 3;
      }
      
      // Private IP check (inlined)
      const uint32_t is_private = (src0 == 10) ||
                                   (src0 == 172 && src1 >= 16 && src1 < 32) ||
                                   (src0 == 192 && src1 == 168);
      
      // Compute key
      const uint32_t key = static_cast<uint32_t>(protocol) * 131 +
                          ttl_bucket * 17 +
                          (is_private ? 5 : 11) +
                          port_bucket * 23;
      
      // FNV-1a hash update
      hash ^= static_cast<uint64_t>(key);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}