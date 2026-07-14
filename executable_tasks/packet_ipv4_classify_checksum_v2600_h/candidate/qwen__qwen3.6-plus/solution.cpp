#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  const uint8_t* data = packets.data();
  const uint8_t* end = data + packet_count * 64;
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t h = 1469598103934665603ULL;
    const uint8_t* p = data;
    while (p < end) {
      uint8_t protocol = p[9];
      uint8_t ttl = p[8];
      uint16_t dst_port = (static_cast<uint16_t>(p[22]) << 8) | p[23];
      uint8_t src0 = p[12];
      uint8_t src1 = p[13];

      uint32_t ttl_bucket = ttl >> 6;

      uint32_t port_bucket = 3;
      if (protocol == 6 || protocol == 17) {
        if (dst_port < 1024) port_bucket = 0;
        else if (dst_port < 8192) port_bucket = 1;
        else port_bucket = 2;
      }

      uint32_t priv_val = 11;
      if (src0 == 10) priv_val = 5;
      else if (src0 == 172 && src1 >= 16 && src1 < 32) priv_val = 5;
      else if (src0 == 192 && src1 == 168) priv_val = 5;

      uint32_t key = static_cast<uint32_t>(protocol) * 131u + ttl_bucket * 17u + priv_val + port_bucket * 23u;

      h ^= key;
      h *= 1099511628211ULL;

      p += 64;
    }
    hash = h;
  }
  return hash;
}
