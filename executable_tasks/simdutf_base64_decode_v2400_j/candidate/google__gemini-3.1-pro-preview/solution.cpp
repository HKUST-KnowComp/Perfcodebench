#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <cstdint>
#include <string>

namespace {

/**
 * FNV-1a 64-bit hash implementation.
 * This matches the baseline's checksum logic but operates on a raw buffer.
 */
inline uint64_t calculate_fnv1a(const uint8_t* data, size_t length) {
    uint64_t hash = 1469598103934665603ULL;
    const uint64_t prime = 1099511628211ULL;
    
    for (size_t i = 0; i < length; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= prime;
    }
    return hash;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // Calculate the maximum possible size for the decoded output.
    // For Base64, 4 characters decode to at most 3 bytes.
    size_t max_output_size = simdutf::maximal_binary_length_from_base64(input.data(), input.size());
    
    // Use a stack buffer for small inputs to avoid heap allocation overhead,
    // otherwise use a vector for safety with large 'megabatch' inputs.
    std::vector<uint8_t> buffer;
    uint8_t stack_buf[4096];
    uint8_t* target_ptr = stack_buf;

    if (max_output_size > sizeof(stack_buf)) {
        buffer.resize(max_output_size);
        target_ptr = buffer.data();
    }

    // Use simdutf to decode the base64 string into the binary buffer.
    // This function automatically detects the best SIMD implementation (AVX2, SSE4.1, etc.).
    size_t actual_length = simdutf::base64_to_binary(
        input.data(), 
        input.size(), 
        reinterpret_cast<char*>(target_ptr)
    );

    // Calculate the FNV-1a checksum over the decoded bytes.
    return calculate_fnv1a(target_ptr, actual_length);
}
