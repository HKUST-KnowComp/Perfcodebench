#include "interface.h"
#include <cstdint>
#include <cstddef>
#include <vector>

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  if (iters == 0) return 0;
  const uint8_t* data = packets.data();
  uint64_t hash = 1469598103934665603ULL;
  constexpr uint64_t kPrime = 1099511628211ULL;
  for (std::size_t i = 0; i < packet_count; ++i) {
    std::size_t offset = i * 64;
    uint8_t ttl = data[offset + 8];
    uint8_t protocol = data[offset + 9];
    uint32_t ttl_bucket = ttl >> 6;
    const uint8_t* src_ip = data + offset + 12;
    bool is_private = (src_ip[0] == 10) ||
                      (src_ip[0] == 172 && src_ip[1] >= 16 && src_ip[1] <= 31) ||
                      (src_ip[0] == 192 && src_ip[1] == 168);
    uint16_t dst_port = (static_cast<uint16_t>(data[offset + 22]) << 8) | data[offset + 23];
    uint32_t port_bucket;
    if (protocol == 6 || protocol == 17) {
      if (dst_port < 1024) port_bucket = 0;
      else if (dst_port < 8192) port_bucket = 1;
      else port_bucket = 2;
    } else {
      port_bucket = 3;
    }
    uint32_t key = static_cast<uint32_t>(protocol) * 131u +
                   ttl_bucket * 17u +
                   (is_private ? 5u : 11u) +
                   port_bucket * 23u;
    hash ^= static_cast<uint64_t>(key);
    hash *= kPrime;
  }
  return hash;
}
