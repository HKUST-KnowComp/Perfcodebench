#include "interface.h"
#include <cstdint>
#include <string>

namespace {
constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

inline uint64_t fnv_step(uint64_t hash, uint8_t byte) {
    return (hash ^ static_cast<uint64_t>(byte)) * 1099511628211ULL;
}
} // namespace

uint64_t encode_checksum(const std::string& input) {
    const size_t n = input.size();
    if (n == 0) return 1469598103934665603ULL;

    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* data = reinterpret_cast<const uint8_t*>(input.data());
    
    size_t i = 0;
    // Process full triples
    if (n >= 3) {
        for (; i <= n - 3; i += 3) {
            uint32_t triple = (static_cast<uint32_t>(data[i]) << 16) |
                             (static_cast<uint32_t>(data[i + 1]) << 8) |
                             (static_cast<uint32_t>(data[i + 2]));
            
            hash = fnv_step(hash, kTable[(triple >> 18) & 0x3F]);
            hash = fnv_step(hash, kTable[(triple >> 12) & 0x3F]);
            hash = fnv_step(hash, kTable[(triple >> 6) & 0x3F]);
            hash = fnv_step(hash, kTable[triple & 0x3F]);
        }
    }

    // Handle remaining 1 or 2 bytes
    if (i < n) {
        uint32_t triple = static_cast<uint32_t>(data[i]) << 16;
        if (i + 1 < n) {
            triple |= static_cast<uint32_t>(data[i + 1]) << 8;
            
            hash = fnv_step(hash, kTable[(triple >> 18) & 0x3F]);
            hash = fnv_step(hash, kTable[(triple >> 12) & 0x3F]);
            hash = fnv_step(hash, kTable[(triple >> 6) & 0x3F]);
            hash = fnv_step(hash, '=');
        } else {
            hash = fnv_step(hash, kTable[(triple >> 18) & 0x3F]);
            hash = fnv_step(hash, kTable[(triple >> 12) & 0x3F]);
            hash = fnv_step(hash, '=');
            hash = fnv_step(hash, '=');
        }
    }

    return hash;
}