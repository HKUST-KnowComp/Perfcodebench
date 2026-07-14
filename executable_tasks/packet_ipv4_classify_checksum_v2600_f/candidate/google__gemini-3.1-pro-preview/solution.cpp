#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

/**
 * Optimized IPv4 packet classification and checksum calculation.
 * 
 * Performance Strategy:
 * 1. Minimize memory access overhead by using direct pointer offsets.
 * 2. Use branchless logic for IP classification and port bucketing.
 * 3. Reduce the number of operations in the hot loop.
 * 4. Maintain the sequential nature of the FNV-1a hash while optimizing the key calculation.
 */

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  
  const uint8_t* const base_ptr = packets.data();
  const uint64_t fnv_prime = 1099511628211ULL;
  const uint64_t fnv_offset_basis = 1469598103934665603ULL;
  
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = fnv_offset_basis;
    
    for (std::size_t i = 0; i < packet_count; ++i) {
      // Each packet is exactly 64 bytes.
      const uint8_t* const pkt = base_ptr + (i << 6);

      // Extract necessary fields directly from the packet buffer.
      const uint8_t ttl = pkt[8];
      const uint8_t protocol = pkt[9];
      const uint8_t ip0 = pkt[12];
      const uint8_t ip1 = pkt[13];
      
      // Big-endian port extraction.
      const uint32_t dst_port = (static_cast<uint32_t>(pkt[22]) << 8) | pkt[23];

      // Branchless TTL bucket: ttl >> 6 (0-63->0, 64-127->1, 128-191->2, 192-255->3).
      const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);

      // Branchless Private IP check:
      // 10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16
      const bool is_private = (ip0 == 10u) |
                              ((ip0 == 172u) & ((ip1 & 0xF0u) == 16u)) |
                              ((ip0 == 192u) & (ip1 == 168u));
      const uint32_t private_key_part = is_private ? 5u : 11u;

      // Branchless Port bucket:
      // If TCP (6) or UDP (17), bucket is based on port ranges.
      // Otherwise, bucket is 3.
      uint32_t port_bucket = 3u;
      if (protocol == 6u || protocol == 17u) {
        // (port >= 1024) is 1 if true, 0 if false. 
        // Summing these conditions gives 0 (<1024), 1 (1024-8191), or 2 (>=8192).
        port_bucket = static_cast<uint32_t>(dst_port >= 1024u) + 
                      static_cast<uint32_t>(dst_port >= 8192u);
      }

      // Calculate classification key.
      const uint32_t key = (static_cast<uint32_t>(protocol) * 131u) +
                           (ttl_bucket * 17u) +
                           private_key_part +
                           (port_bucket * 23u);

      // Update FNV-1a hash.
      hash ^= static_cast<uint64_t>(key);
      hash *= fnv_prime;
    }
  }

  return hash;
}
