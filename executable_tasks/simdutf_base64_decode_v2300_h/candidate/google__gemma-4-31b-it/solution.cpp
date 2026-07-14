#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

uint64_t decode_checksum(const std::string& input) {
    if (input.empty()) return 1469598103934665603ULL;

    // Calculate the maximum possible decoded size
    size_t max_decoded_size = (input.size() * 3) / 4;
    
    // Use a stack buffer for small inputs to avoid heap allocation
    char stack_buf[4096];
    char* out_ptr = stack_buf;
    std::vector<char> heap_buf;

    if (max_decoded_size > sizeof(stack_buf)) {
        heap_buf.resize(max_decoded_size);
        out_ptr = heap_buf.data();
    }

    // Use simdutf for high-performance base64 decoding
    auto result = simdutf::base64::decode(input.data(), input.size(), out_ptr);
    
    if (result.error) {
        // In case of decoding error, the contract implies we should handle it.
        // Based on the baseline, we assume valid base64 or return 0/current hash.
        return 0;
    }

    // Compute FNV-1a checksum over the decoded bytes
    uint64_t hash = 1469598103934665603ULL;
    const uint8_t* data = reinterpret_cast<const uint8_t*>(out_ptr);
    size_t length = result.decoded_size;

    for (size_t i = 0; i < length; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }

    return hash;
}