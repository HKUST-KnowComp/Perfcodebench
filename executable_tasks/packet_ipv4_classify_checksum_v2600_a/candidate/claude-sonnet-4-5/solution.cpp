#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr std::size_t kPacketSize = 64;

// Precomputed lookup table for private IP first octet
static const bool private_ip_first_octet[256] = {
    false, false, false, false, false, false, false, false, false, false,
    true,  false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, true,  false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, true,  false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false, false, false, false, false,
    false, false, false, false, false, false
};

inline bool is_private_ip(const uint8_t* ip) {
  const uint8_t first = ip[0];
  if (!private_ip_first_octet[first]) return false;
  
  if (first == 10u) return true;
  if (first == 172u) return ip[1] >= 16u && ip[1] < 32u;
  return ip[1] == 168u; // first == 192
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
      const uint8_t* src = pkt + 12;
      
      const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);
      
      uint32_t port_bucket;
      if (protocol == 6u || protocol == 17u) {
        const uint16_t dst_port = static_cast<uint16_t>((static_cast<uint16_t>(pkt[22]) << 8) | pkt[23]);
        port_bucket = (dst_port < 1024u) ? 0u : ((dst_port < 8192u) ? 1u : 2u);
      } else {
        port_bucket = 3u;
      }
      
      const uint32_t src_class = is_private_ip(src) ? 5u : 11u;
      
      const uint32_t key = static_cast<uint32_t>(protocol) * 131u + 
                           ttl_bucket * 17u + 
                           src_class + 
                           port_bucket * 23u;
      
      hash ^= static_cast<uint64_t>(key);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}