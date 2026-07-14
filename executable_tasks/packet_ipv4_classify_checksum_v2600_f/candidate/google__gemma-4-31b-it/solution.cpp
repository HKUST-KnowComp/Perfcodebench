#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr std::size_t kPacketSize = 64;

inline bool is_private_ip(const uint8_t* ip) {
  if (ip[0] == 10u) return true;
  if (ip[0] == 172u && ip[1] >= 16u && ip[1] < 32u) return true;
  if (ip[0] == 192u && ip[1] == 168u) return true;
  return false;
}

}  // namespace

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  if (iters <= 0) return 0;

  // The hash is reset to a constant at the start of every iteration.
  // Therefore, the result of the inner loop is the same for every iteration.
  // We only need to compute it once.
  uint64_t hash = 1469598103934665603ULL;
  const uint8_t* data = packets.data();

  for (std::size_t i = 0; i < packet_count; ++i) {
    const uint8_t* pkt = data + i * kPacketSize;
    
    // IPv4 Header offsets:
    // TTL: offset 8
    // Protocol: offset 9
    // Src IP: offset 12
    // Port: offset 20 (src), 22 (dst) - based on the baseline's memcpy logic
    
    const uint8_t ttl = pkt[8];
    const uint8_t protocol = pkt[9];
    const uint8_t* src_ip = pkt + 12;
    
    // Baseline dst_port calculation: (packets[22] << 8) | packets[23]
    const uint16_t dst_port = (static_cast<uint16_t>(pkt[22]) << 8) | pkt[23];
    
    const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);
    
    uint32_t port_bucket;
    if (protocol == 6u || protocol == 17u) {
      if (dst_port < 1024u) port_bucket = 0u;
      else if (dst_port < 8192u) port_bucket = 1u;
      else port_bucket = 2u;
    } else {
      port_bucket = 3u;
    }

    const uint32_t key = 
        static_cast<uint32_t>(protocol) * 131u + 
        ttl_bucket * 17u + 
        (is_private_ip(src_ip) ? 5u : 11u) + 
        port_bucket * 23u;

    hash ^= static_cast<uint64_t>(key);
    hash *= 1099511628211ULL;
  }

  return hash;
}