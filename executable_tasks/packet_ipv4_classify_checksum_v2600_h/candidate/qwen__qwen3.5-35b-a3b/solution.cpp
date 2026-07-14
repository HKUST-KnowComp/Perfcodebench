#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr std::size_t kPacketSize = 64;

// Branchless private IP check for 10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16
inline bool is_private_ip(const uint8_t* ip) {
  const uint8_t b0 = ip[0];
  const uint8_t b1 = ip[1];
  const uint32_t is_10 = (b0 == 10u) ? 1u : 0u;
  const uint32_t is_172 = (b0 == 172u) ? 1u : 0u;
  const uint32_t is_192 = (b0 == 192u) ? 1u : 0u;
  const uint32_t is_172_16_31 = (is_172 && b1 >= 16u && b1 < 32u) ? 1u : 0u;
  const uint32_t is_192_168 = (is_192 && b1 == 168u) ? 1u : 0u;
  return (is_10 || is_172_16_31 || is_192_168) != 0;
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
      const std::size_t offset = i * kPacketSize;
      
      // Read fields directly without memcpy
      const uint8_t ttl = data[offset + 8];
      const uint8_t protocol = data[offset + 9];
      const uint8_t src0 = data[offset + 12];
      const uint8_t src1 = data[offset + 13];
      const uint8_t src2 = data[offset + 14];
      const uint8_t src3 = data[offset + 15];
      const uint8_t dst_port_high = data[offset + 22];
      const uint8_t dst_port_low = data[offset + 23];
      const uint16_t dst_port = static_cast<uint16_t>((dst_port_high << 8) | dst_port_low);
      
      // TTL bucket: ttl >> 6
      const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);
      
      // Port bucket
      const uint32_t port_bucket =
          (protocol == 6u || protocol == 17u) ? 
              ((dst_port < 1024u) ? 0u : ((dst_port < 8192u) ? 1u : 2u)) : 3u;
      
      // Private IP check
      const uint32_t is_private = is_private_ip(&data[offset + 12]) ? 5u : 11u;
      
      // Compute key
      const uint32_t key =
          static_cast<uint32_t>(protocol) * 131u + ttl_bucket * 17u +
          is_private + port_bucket * 23u;
      
      // FNV-1a hash
      hash ^= static_cast<uint64_t>(key);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}