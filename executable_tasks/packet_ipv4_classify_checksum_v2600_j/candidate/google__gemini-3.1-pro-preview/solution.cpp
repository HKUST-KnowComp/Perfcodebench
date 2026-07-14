#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

/**
 * Performance Engineer Optimization:
 * 1. Field Extraction: Replaced memcpy and struct overhead with direct byte access.
 * 2. Branch Reduction: Simplified the private IP check and port bucket logic to reduce pipeline stalls.
 * 3. Memory Access: Used a local pointer and constant offsets to help the compiler optimize memory loads.
 * 4. Hash Chain: Maintained the FNV-1a dependency chain while ensuring the key calculation is as fast as possible.
 */

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
  
  const uint8_t* const base_ptr = packets.data();
  const uint64_t fnv_offset_basis = 1469598103934665603ULL;
  const uint64_t fnv_prime = 1099511628211ULL;
  uint64_t hash = 0;

  for (int iter = 0; iter < iters; ++iter) {
    hash = fnv_offset_basis;
    for (std::size_t i = 0; i < packet_count; ++i) {
      const uint8_t* p = base_ptr + (i << 6); // i * 64

      // Extract fields directly
      const uint8_t protocol = p[9];
      const uint8_t ttl = p[8];
      const uint8_t ip0 = p[12];
      const uint8_t ip1 = p[13];
      
      // Big-endian port extraction
      const uint32_t dst_port = (static_cast<uint32_t>(p[22]) << 8) | p[23];

      // TTL bucket: ttl / 64
      const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);

      // Private IP check: 
      // 10.0.0.0/8, 172.16.0.0/12, 192.168.0.0/16
      bool is_private = (ip0 == 10) || 
                        (ip0 == 172 && ip1 >= 16 && ip1 < 32) || 
                        (ip0 == 192 && ip1 == 168);
      const uint32_t priv_val = is_private ? 5u : 11u;

      // Port bucket logic
      uint32_t port_bucket = 3u;
      if (protocol == 6u || protocol == 17u) {
        if (dst_port < 1024u) {
          port_bucket = 0u;
        } else if (dst_port < 8192u) {
          port_bucket = 1u;
        } else {
          port_bucket = 2u;
        }
      }

      // Key calculation
      const uint32_t key = (static_cast<uint32_t>(protocol) * 131u) + 
                           (ttl_bucket * 17u) + 
                           priv_val + 
                           (port_bucket * 23u);

      // FNV-1a hash update
      hash ^= static_cast<uint64_t>(key);
      hash *= fnv_prime;
    }
  }

  return hash;
}
