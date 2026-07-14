#include "interface.h"

#include <cstddef>
#include <cstdint>

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  constexpr std::size_t kPacketSize = 64;
  const uint8_t* __restrict__ data = packets.data();
  uint64_t hash = 0;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    const uint8_t* pkt = data;
    
    for (std::size_t i = 0; i < packet_count; ++i) {
      const uint8_t ttl = pkt[8];
      const uint8_t protocol = pkt[9];
      const uint8_t src0 = pkt[12];
      const uint8_t src1 = pkt[13];
      const uint16_t dst_port = static_cast<uint16_t>((static_cast<uint16_t>(pkt[22]) << 8) | pkt[23]);
      
      const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);
      
      uint32_t port_bucket;
      if (protocol == 6u || protocol == 17u) {
        port_bucket = (dst_port < 1024u) ? 0u : ((dst_port < 8192u) ? 1u : 2u);
      } else {
        port_bucket = 3u;
      }
      
      const bool is_private = (src0 == 10u) |
                              ((src0 == 172u) & (src1 >= 16u) & (src1 < 32u)) |
                              ((src0 == 192u) & (src1 == 168u));
      
      const uint32_t key =
          static_cast<uint32_t>(protocol) * 131u + ttl_bucket * 17u +
          (is_private ? 5u : 11u) + port_bucket * 23u;
      
      hash ^= static_cast<uint64_t>(key);
      hash *= 1099511628211ULL;
      
      pkt += kPacketSize;
    }
  }
  return hash;
}