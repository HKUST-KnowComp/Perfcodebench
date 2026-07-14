#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>
#include <cstdint>

namespace {

// FNV-1a constants for 64-bit hashing
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline uint64_t compute_fnv1a(const char* data, size_t len) {
    uint64_t hash = FNV_OFFSET_BASIS;
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(data);
    const uint8_t* end = ptr + len;
    
    // Unroll the loop to improve instruction-level parallelism
    while (end - ptr >= 8) {
        hash ^= static_cast<uint64_t>(ptr[0]); hash *= FNV_PRIME;
        hash ^= static_cast<uint64_t>(ptr[1]); hash *= FNV_PRIME;
        hash ^= static_cast<uint64_t>(ptr[2]); hash *= FNV_PRIME;
        hash ^= static_cast<uint64_t>(ptr[3]); hash *= FNV_PRIME;
        hash ^= static_cast<uint64_t>(ptr[4]); hash *= FNV_PRIME;
        hash ^= static_cast<uint64_t>(ptr[5]); hash *= FNV_PRIME;
        hash ^= static_cast<uint64_t>(ptr[6]); hash *= FNV_PRIME;
        hash ^= static_cast<uint64_t>(ptr[7]); hash *= FNV_PRIME;
        ptr += 8;
    }
    while (ptr < end) {
        hash ^= static_cast<uint64_t>(*ptr++);
        hash *= FNV_PRIME;
    }
    return hash;
}

} // namespace

uint64_t encode_checksum(const std::string& input) {
    if (input.empty()) {
        return compute_fnv1a(nullptr, 0);
    }

    // Calculate required base64 length
    size_t expected_len = simdutf::base64_length_from_binary(input.size());
    
    // Use a stack-allocated buffer for small inputs or a vector for large ones
    // to avoid std::string overhead and ensure contiguous memory.
    std::vector<char> buffer(expected_len);

    // Use simdutf to perform high-speed SIMD base64 encoding.
    // This handles padding and the standard base64 alphabet.
    size_t actual_len = simdutf::binary_to_base64(input.data(), input.size(), buffer.data());

    // Compute the FNV-1a checksum over the resulting base64 bytes.
    return compute_fnv1a(buffer.data(), actual_len);
}