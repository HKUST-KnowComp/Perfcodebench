#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr std::size_t kPacketSize = 64;

// Optimized private IP check
inline uint32_t get_ip_class_val(const uint8_t* ip) {
    if (ip[0] == 10) return 5;
    if (ip[0] == 192 && ip[1] == 168) return 5;
    if (ip[0] == 172 && ip[1] >= 16 && ip[1] < 32) return 5;
    return 11;
}

} // namespace

uint64_t classify_packets_checksum(
    const std::vector<uint8_t>& packets,
    std::size_t packet_count,
    int iters) {
    
    const uint8_t* data = packets.data();
    uint64_t hash = 0;

    for (int iter = 0; iter < iters; ++iter) {
        hash = 1469598103934665603ULL;
        for (std::size_t i = 0; i < packet_count; ++i) {
            const uint8_t* pkt = data + (i * kPacketSize);
            
            // IPv4 Header offsets:
            // TTL: offset 8
            // Protocol: offset 9
            // Src IP: offset 12
            // Ports: offset 20 (src), 22 (dst)
            
            const uint8_t protocol = pkt[9];
            const uint8_t ttl = pkt[8];
            const uint8_t* src_ip = pkt + 12;
            
            // dst_port is at offset 22, 23 (Big Endian)
            const uint16_t dst_port = (static_cast<uint16_t>(pkt[22]) << 8) | pkt[23];
            
            const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);
            
            uint32_t port_bucket;
            if (protocol == 6 || protocol == 17) {
                if (dst_port < 1024) port_bucket = 0;
                else if (dst_port < 8192) port_bucket = 1;
                else port_bucket = 2;
            } else {
                port_bucket = 3;
            }

            const uint32_t key = static_cast<uint32_t>(protocol) * 131u + 
                                 ttl_bucket * 17u + 
                                 get_ip_class_val(src_ip) + 
                                 port_bucket * 23u;
            
            hash ^= static_cast<uint64_t>(key);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}