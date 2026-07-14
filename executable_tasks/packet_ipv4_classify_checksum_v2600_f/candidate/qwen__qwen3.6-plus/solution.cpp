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
  const uint64_t FNV_PRIME = 1099511628211ULL;
  const uint64_t FNV_OFFSET = 1469598103934665603ULL;

  for (int iter = 0; iter < iters; ++iter) {
    hash = FNV_OFFSET;
    const uint8_t* p = data;
    for (std::size_t i = 0; i < packet_count; ++i) {
      uint8_t protocol = p[9];
      uint8_t ttl = p[8];
      uint16_t dst_port = (static_cast<uint16_t>(p[22]) << 8) | p[23];
      
      uint32_t ttl_bucket = ttl >> 6;
      
      uint32_t is_tcp_udp = (protocol == 6) | (protocol == 17);
      uint32_t port_bucket = is_tcp_udp ? ((dst_port >= 1024) + (dst_port >= 8192)) : 3;
      
      uint8_t s0 = p[12];
      uint8_t s1 = p[13];
      uint32_t is_priv = (s0 == 10) | ((s0 == 172) & (s1 >= 16) & (s1 < 32)) | ((s0 == 192) & (s1 == 168));
      uint32_t priv_val = 11 - 6 * is_priv;
      
      uint32_t key = static_cast<uint32_t>(protocol) * 131u + ttl_bucket * 17u + priv_val + port_bucket * 23u;
      hash ^= static_cast<uint64_t>(key);
      hash *= FNV_PRIME;
      
      p += 64;
    }
  }
  return hash;
}