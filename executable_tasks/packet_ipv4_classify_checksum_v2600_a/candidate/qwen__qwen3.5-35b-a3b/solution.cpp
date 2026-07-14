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
  const std::size_t packet_size = 64;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < packet_count; ++i) {
      const std::size_t offset = i * packet_size;
      
      // Extract fields directly without memcpy overhead
      const uint8_t ttl = data[offset + 8];
      const uint8_t protocol = data[offset + 9];
      const uint8_t src0 = data[offset + 12];
      const uint8_t src1 = data[offset + 13];
      const uint16_t dst_port = static_cast<uint16_t>(data[offset + 22] << 8 | data[offset + 23]);
      
      // TTL bucket (top 2 bits)
      const uint32_t ttl_bucket = ttl >> 6;
      
      // Port bucket - inline logic
      uint32_t port_bucket;
      if (protocol == 6 || protocol == 17) {
        port_bucket = (dst_port < 1024u) ? 0u : ((dst_port < 8192u) ? 1u : 2u);
      } else {
        port_bucket = 3u;
      }
      
      // Private IP check - inline logic
      uint32_t src_class = 11u;
      if (src0 == 10u) {
        src_class = 5u;
      } else if (src0 == 172u && src1 >= 16u && src1 < 32u) {
        src_class = 5u;
      } else if (src0 == 192u && src1 == 168u) {
        src_class = 5u;
      }
      
      // Compute key
      const uint32_t key = static_cast<uint32_t>(protocol) * 131u + ttl_bucket * 17u + src_class + port_bucket * 23u;
      
      // FNV-1a hash update
      hash ^= static_cast<uint64_t>(key);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}