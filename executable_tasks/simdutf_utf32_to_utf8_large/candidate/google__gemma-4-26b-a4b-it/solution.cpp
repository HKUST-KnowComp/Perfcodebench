#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

namespace {

// FNV-1a hash implementation as required by the baseline logic
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

uint64_t transcode_checksum(const std::vector<char32_t>& input) {
    if (input.empty()) {
        return checksum_bytes("");
    }

    // Use simdutf for high-performance SIMD-accelerated transcoding
    // simdutf::convert_utf32_to_utf8 returns a std::string
    std::string out = simdutf::convert_utf32_to_utf8(reinterpret_cast<const char32_t*>(input.data()), input.size());
    
    return checksum_bytes(out);
}