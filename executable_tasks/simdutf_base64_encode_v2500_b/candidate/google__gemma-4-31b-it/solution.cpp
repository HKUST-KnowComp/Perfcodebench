#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <string>

namespace {

// FNV-1a 64-bit constants
constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
constexpr uint64_t FNV_PRIME = 1099511628211ULL;

// Helper to update checksum from a buffer
inline void update_checksum(uint64_t& hash, const char* data, size_t len) {
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= FNV_PRIME;
    }
}

} // namespace

uint64_t encode_checksum(const std::string& input) {
    if (input.empty()) {
        return FNV_OFFSET_BASIS;
    }

    // Calculate required size for base64 output
    size_t input_len = input.size();
    size_t output_len = simdutf::base64_encoded_size(input_len);
    
    // Use a stack buffer for small inputs to avoid heap allocation
    char stack_buf[1024];
    const char* out_ptr = stack_buf;
    std::vector<char> heap_buf;

    if (output_len <= 1024) {
        out_ptr = stack_buf;
    } else {
        heap_buf.resize(output_len);
        out_ptr = heap_buf.data();
    }

    // Perform SIMD accelerated encoding
    simdutf::base64_encode(input.data(), input_len, out_ptr, output_len);

    // Compute checksum directly on the result
    uint64_t hash = FNV_OFFSET_BASIS;
    update_checksum(hash, out_ptr, output_len);
    
    return hash;
}