#pragma once

#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

inline bool is_private_ip(const uint8_t* ip) {
  return ip[0] == 10u ||
         (ip[0] == 172u && ip[1] >= 16u && ip[1] < 32u) ||
         (ip[0] == 192u && ip[1] == 168u);
}

}  // namespace

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  uint64_t hash = 0;
  const uint8_t* data = packets.data();
  const std::size_t packet_size = 64;
  
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    for (std::size_t i = 0; i < packet_count; ++i) {
      const std::size_t offset = i * packet_size;
      
      // Extract fields directly from packet data
      const uint8_t version_ihl = data[offset];
      const uint8_t protocol = data[offset + 9];
      const uint8_t ttl = data[offset + 8];
      const uint8_t src0 = data[offset + 12];
      const uint8_t src1 = data[offset + 13];
      const uint8_t src2 = data[offset + 14];
      const uint8_t src3 = data[offset + 15];
      const uint8_t dst_port_high = data[offset + 22];
      const uint8_t dst_port_low = data[offset + 23];
      
      // Calculate destination port once
      const uint16_t dst_port = static_cast<uint16_t>((dst_port_high << 8) | dst_port_low);
      
      // TTL bucket: ttl >> 6
      const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);
      
      // Port bucket based on protocol and destination port
      uint32_t port_bucket;
      if (protocol == 6u || protocol == 17u) {
        port_bucket = (dst_port < 1024u) ? 0u : ((dst_port < 8192u) ? 1u : 2u);
      } else {
        port_bucket = 3u;
      }
      
      // Private IP check
      const uint32_t src_private = is_private_ip(data + offset + 12) ? 5u : 11u;
      
      // Calculate key
      const uint32_t key =
          static_cast<uint32_t>(protocol) * 131u +
          ttl_bucket * 17u +
          src_private +
          port_bucket * 23u;
      
      // FNV-1a hash update
      hash ^= static_cast<uint64_t>(key);
      hash *= 1099511628211ULL;
    }
  }
  return hash;
}
