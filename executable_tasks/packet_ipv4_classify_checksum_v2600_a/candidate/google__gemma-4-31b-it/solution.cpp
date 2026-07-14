#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr std::size_t kPacketSize = 64;

// Optimized private IP check using a 32-bit integer representation for faster comparison
// However, since the input is bytes, we keep it simple but avoid redundant indexing.
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
            // Src IP: offset 12-15
            // Src Port: offset 20-21 (assuming fixed offset for this synthetic task)
            // Dst Port: offset 22-23
            
            const uint8_t ttl = pkt[8];
            const uint8_t protocol = pkt[9];
            const uint8_t* src_ip = &pkt[12];
            
            // Extract dst_port (Big Endian)
            const uint16_t dst_port = (static_cast<uint16_t>(pkt[22]) << 8) | pkt[23];
            
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
        }
    }
    return hash;
}