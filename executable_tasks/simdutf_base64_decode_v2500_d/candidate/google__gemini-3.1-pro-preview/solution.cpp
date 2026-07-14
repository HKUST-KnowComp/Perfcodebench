#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>
#include <cstdint>

namespace {

// FNV-1a constants
static constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
static constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline uint64_t compute_fnv1a(const char* data, size_t length) {
    uint64_t hash = FNV_OFFSET_BASIS;
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(data);
    const uint8_t* end = ptr + length;
    while (ptr < end) {
        hash ^= static_cast<uint64_t>(*ptr++);
        hash *= FNV_PRIME;
    }
    return hash;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
    if (input.empty()) {
        return FNV_OFFSET_BASIS;
    }

    // Calculate maximum possible output size
    // Base64 maps 4 chars to 3 bytes. 
    size_t max_output_size = simdutf::maximal_binary_length_from_base64(input.data(), input.length());
    
    // Use a stack buffer for small inputs or a vector for large ones to avoid overhead
    // However, for simplicity and safety in a performance context, we use a vector.
    std::vector<char> buffer(max_output_size);

    // Use simdutf to decode. It handles padding and various base64 variants efficiently.
    // base64_to_binary returns the actual number of bytes written.
    simdutf::result res = simdutf::base64_to_binary(input.data(), input.length(), buffer.data());
    
    if (res.error != simdutf::error_code::SUCCESS) {
        // In case of error, we fall back to a safe result or handle as per contract.
        // Given the task, we assume valid base64 or best-effort.
        return compute_fnv1a(buffer.data(), res.count);
    }

    return compute_fnv1a(buffer.data(), res.count);
}
