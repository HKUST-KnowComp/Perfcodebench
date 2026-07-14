#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr std::size_t kPacketSize = 64;

// Optimized private IP check using a pointer to the source IP address
inline uint32_t get_ip_class_val(const uint8_t* ip) {
    if (ip[0] == 10u) return 5u;
    if (ip[0] == 192u && ip[1] == 168u) return 5u;
    if (ip[0] == 172u && ip[1] >= 16u && ip[1] < 32u) return 5u;
    return 11u;
}

} // namespace

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
    
    const uint8_t* data_ptr = packets.data();
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        const uint8_t* curr = data_ptr;
        
        for (std::size_t i = 0; i < packet_count; ++i) {
            // IPv4 Header offsets:
            // TTL: offset 8
            // Protocol: offset 9
            // Src IP: offset 12
            // Dst Port: offset 22 (assuming fixed offset for this synthetic task)
            
            const uint8_t protocol = curr[9];
            const uint8_t ttl = curr[8];
            const uint8_t* src_ip = &curr[12];
            
            // Extract dst_port (Big Endian)
            const uint16_t dst_port = (static_cast<uint16_t>(curr[22]) << 8) | curr[23];
            
            const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);
            
            uint32_t port_bucket;
            if (protocol == 6u || protocol == 17u) {
                if (dst_port < 1024u) port_bucket = 0u;
                else if (dst_port < 8192u) port_bucket = 1u;
                else port_bucket = 2u;
            } else {
                port_bucket = 3u;
            }
            
            const uint32_t key = static_cast<uint32_t>(protocol) * 131u +
                                 ttl_bucket * 17u +
                                 get_ip_class_val(src_ip) +
                                 port_bucket * 23u;
            
            hash ^= static_cast<uint64_t>(key);
            hash *= 1099511628211ULL;
            
            curr += kPacketSize;
        }
    }
    return hash;
}