#include "interface.h"
#include "simdutf.h"
#include <vector>

namespace {

// Optimized FNV-1a checksum implementation
inline uint64_t checksum_bytes(const std::string& data) {
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* ptr = reinterpret_cast<const unsigned char*>(data.data());
    size_t len = data.size();
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(ptr[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // Use simdutf for high-performance SIMD-accelerated transcoding
    // simdutf::convert_latin1_to_utf8 returns a std::string
    std::string utf8_output = simdutf::convert_latin1_to_utf8(input.data(), input.size());
    
    return checksum_bytes(utf8_output);
}
