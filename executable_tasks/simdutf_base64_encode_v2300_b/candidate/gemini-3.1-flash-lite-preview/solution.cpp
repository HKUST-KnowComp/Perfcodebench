#include "interface.h"
#include <cstdint>
#include <string>

namespace {
constexpr char kTable[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

inline uint64_t update_hash(uint64_t hash, char ch) {
    return (hash ^ static_cast<uint64_t>(static_cast<unsigned char>(ch))) * 1099511628211ULL;
}
} // namespace

uint64_t encode_checksum(const std::string& input) {
    uint64_t hash = 1469598103934665603ULL;
    const size_t n = input.size();
    size_t i = 0;

    for (; i + 2 < n; i += 3) {
        uint32_t b0 = static_cast<unsigned char>(input[i]);
        uint32_t b1 = static_cast<unsigned char>(input[i + 1]);
        uint32_t b2 = static_cast<unsigned char>(input[i + 2]);
        uint32_t triple = (b0 << 16) | (b1 << 8) | b2;

        hash = update_hash(hash, kTable[(triple >> 18) & 0x3F]);
        hash = update_hash(hash, kTable[(triple >> 12) & 0x3F]);
        hash = update_hash(hash, kTable[(triple >> 6) & 0x3F]);
        hash = update_hash(hash, kTable[triple & 0x3F]);
    }

    if (i < n) {
        uint32_t b0 = static_cast<unsigned char>(input[i]);
        uint32_t b1 = (i + 1 < n) ? static_cast<unsigned char>(input[i + 1]) : 0;
        uint32_t triple = (b0 << 16) | (b1 << 8);

        hash = update_hash(hash, kTable[(triple >> 18) & 0x3F]);
        hash = update_hash(hash, kTable[(triple >> 12) & 0x3F]);
        hash = update_hash(hash, (i + 1 < n) ? kTable[(triple >> 6) & 0x3F] : '=');
        hash = update_hash(hash, '=');
    }

    return hash;
}