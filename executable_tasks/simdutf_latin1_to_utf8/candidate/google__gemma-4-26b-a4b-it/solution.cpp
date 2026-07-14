#include "interface.h"
#include "simdutf.h"
#include <vector>

namespace {

// FNV-1a hash implementation
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

    // Latin-1 to UTF-8 can expand at most to 2x the size
    // simdutf::convert_latin1_to_utf8 is highly optimized using SIMD instructions
    std::string output;
    output.resize(input.size() * 2); 
    
    size_t encoded_size = simdutf::convert_latin1_to_utf8(
        reinterpret_cast<const uint8_t*>(input.data()), 
        input.size(), 
        reinterpret_cast<uint8_t*>(&output[0])
    );
    
    // Resize to the actual encoded size to ensure checksum is correct
    output.resize(encoded_size);

    return checksum_bytes(output);
}
