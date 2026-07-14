#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>

uint64_t decode_checksum(const std::string& input) {
    if (input.empty()) return 1469598103934665603ULL;

    // Calculate max possible output size
    size_t max_len = (input.size() / 4) * 3;
    std::vector<uint8_t> buffer(max_len);

    // Use simdutf to decode base64
    simdutf::result res = simdutf::base64_to_binary(
        input.data(), 
        input.size(), 
        buffer.data(), 
        simdutf::base64_options::default_options
    );

    size_t decoded_size = res.count;
    
    // FNV-1a checksum calculation
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* data = buffer.data();
    for (size_t i = 0; i < decoded_size; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    
    return hash;
}