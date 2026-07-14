#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  uint64_t hash = 0;
  const uint8_t* pkt = packets.data();
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < packet_count; ++i) {
      uint8_t protocol = pkt[9];
      uint8_t ttl = pkt[8];
      const uint8_t* src = pkt + 12;
      uint16_t dst_port = (pkt[22] << 8) | pkt[23];

      uint32_t ttl_bucket = ttl >> 6;

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

      bool is_private = (src[0] == 10) ||
                        (src[0] == 172 && src[1] >= 16 && src[1] < 32) ||
                        (src[0] == 192 && src[1] == 168);

      uint32_t key = protocol * 131 + ttl_bucket * 17 + (is_private ? 5 : 11) + port_bucket * 23;

      hash ^= key;
      hash *= 1099511628211ULL;

      pkt += 64;
    }
  }
  return hash;
}