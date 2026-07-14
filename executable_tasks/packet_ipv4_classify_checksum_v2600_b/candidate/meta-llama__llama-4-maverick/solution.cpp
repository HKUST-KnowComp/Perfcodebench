#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr std::size_t kPacketSize = 64;

uint32_t is_private_ip(const uint8_t* ip) {
  return (ip[0] == 10u) | ((ip[0] == 172u && (ip[1] >= 16u && ip[1] < 32u)) << 1) | ((ip[0] == 192u && ip[1] == 168u) << 2);
}

uint32_t classify_packet(const uint8_t* packet) {
  uint8_t ttl = packet[8];
  uint8_t protocol = packet[9];
  uint16_t dst_port = (static_cast<uint16_t>(packet[22]) << 8) | packet[23];
  uint32_t ttl_bucket = ttl >> 6;
  uint32_t port_bucket = (protocol == 6 || protocol == 17) ? ((dst_port < 1024) ? 0 : ((dst_port < 8192) ? 1 : 2)) : 3;
  uint32_t src_class = is_private_ip(packet + 12);
  return protocol * 131 + ttl_bucket * 17 + (src_class != 0) * (src_class << 2) + port_bucket * 23;
}

}  // namespace

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    const uint8_t* packet_data = packets.data();
    for (std::size_t i = 0; i < packet_count; ++i) {
      uint32_t key = classify_packet(packet_data + i * kPacketSize);
      hash ^= static_cast<uint64_t>(key);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}