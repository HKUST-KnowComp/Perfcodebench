#include "interface.h"

#include <cstddef>
#include <cstdint>
#include <vector>

namespace {

constexpr std::size_t kPacketSize = 64;

inline bool is_private_ip_fast(const uint8_t* ip) {
    uint8_t p0 = ip[0];
    if (p0 == 10) return true;
    if (p0 == 172) return ip[1] >= 16 && ip[1] < 32;
    if (p0 == 192) return ip[1] == 168;
    return false;
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
            const uint8_t* p = data + (i * kPacketSize);
            
            // IPv4 Header fields extraction
            // p[8] is TTL, p[9] is Protocol, p[12..15] is Src IP
            const uint8_t ttl = p[8];
            const uint8_t protocol = p[9];
            const uint8_t* src_ip = p + 12;
            
            // Port extraction (p[22..23] is dst_port)
            const uint16_t dst_port = static_cast<uint16_t>((p[22] << 8) | p[23]);

            const uint32_t ttl_bucket = static_cast<uint32_t>(ttl >> 6);
            
            uint32_t port_bucket;
            if (protocol == 6 || protocol == 17) {
                if (dst_port < 1024) port_bucket = 0;
                else if (dst_port < 8192) port_bucket = 1;
                else port_bucket = 2;
            } else {
                port_bucket = 3;
            }

            const uint32_t ip_val = is_private_ip_fast(src_ip) ? 5u : 11u;
            
            const uint32_t key = static_cast<uint32_t>(protocol) * 131u + 
                                 ttl_bucket * 17u + 
                                 ip_val + 
                                 port_bucket * 23u;

            hash ^= static_cast<uint64_t>(key);
            hash *= 1099511628211ULL;
        }
    }
    return hash;
}