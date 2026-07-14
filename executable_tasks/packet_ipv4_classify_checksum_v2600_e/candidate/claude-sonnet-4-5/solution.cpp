#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr std::size_t kPacketSize = 64;

inline bool is_private_ip(uint8_t b0, uint8_t b1) {
  return b0 == 10u ||
         (b0 == 172u && b1 >= 16u && b1 < 32u) ||
         (b0 == 192u && b1 == 168u);
}

}  // namespace

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  uint64_t hash = 0;
  const uint8_t* data = packets.data();
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    
    for (std::size_t i = 0; i < packet_count; ++i) {
      const uint8_t* pkt = data + i * kPacketSize;
      
      const uint8_t ttl = pkt[8];
      const uint8_t protocol = pkt[9];
      const uint8_t src0 = pkt[12];
      const uint8_t src1 = pkt[13];
      const uint16_t dst_port = static_cast<uint16_t>((static_cast<uint16_t>(pkt[22]) << 8) | pkt[23]);
      
      const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);
      const uint32_t port_bucket =
          (protocol == 6u || protocol == 17u) ? ((dst_port < 1024u) ? 0u : ((dst_port < 8192u) ? 1u : 2u)) : 3u;
      const uint32_t key =
          static_cast<uint32_t>(protocol) * 131u + ttl_bucket * 17u +
          (is_private_ip(src0, src1) ? 5u : 11u) + port_bucket * 23u;
      
      hash ^= static_cast<uint64_t>(key);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}