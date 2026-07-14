#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

namespace {

// FNV-1a hash implementation as per the original logic
uint64_t checksum_bytes(const std::string& data) {
    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : data) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
    if (input.empty()) {
        return checksum_bytes("");
    }

    // simdutf::base64::unpack_padded handles standard base64 with padding.
    // It is significantly faster than scalar implementations due to SIMD usage.
    // We use a vector to manage the decoded buffer memory.
    std::vector<uint8_t> decoded_buffer;
    
    // simdutf::base64::unpack_padded returns the number of bytes written.
    // We need to pre-allocate or use a container that can receive the output.
    // Since the interface requires a checksum over the exact bytes, 
    // we use the simdutf API to decode into a buffer.
    
    // Note: simdutf::base64::unpack_padded is designed for high performance.
    // We use a temporary buffer to hold the decoded data.
    size_t max_out_size = (input.size() / 4) * 3;
    decoded_buffer.resize(max_out_size);

    size_t decoded_size = simdutf::base64::unpack_padded(input.data(), input.size(), decoded_buffer.data());
    
    // Create a string view or a temporary string to pass to the checksum function.
    // To avoid extra copies, we can modify checksum_bytes to take a pointer/size, 
    // but to strictly follow the 'preserve correctness' and 'interface' rules 
    // while using the existing checksum_bytes signature:
    std::string result_str(reinterpret_cast<char*>(decoded_buffer.data()), decoded_size);
    
    return checksum_bytes(result_str);
}