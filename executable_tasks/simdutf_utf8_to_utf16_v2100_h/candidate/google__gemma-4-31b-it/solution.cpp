#include "interface.h"
#include "simdutf.h"
#include <vector>

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // The maximum possible size for UTF-16 output is input.size() (for ASCII)
    // or slightly more for certain multi-byte sequences, but generally 
    // input.size() is a safe upper bound for the number of char16_t units 
    // unless the input is very small and contains many 4-byte sequences.
    // However, simdutf::convert_utf8_to_utf16 returns the number of units written.
    
    std::vector<char16_t> buffer(input.size() + 1); 
    
    // simdutf::convert_utf8_to_utf16 is highly optimized using SIMD
    auto result = simdutf::convert_utf8_to_utf16(
        reinterpret_cast<const char*>(input.data()), 
        input.size(), 
        buffer.data(), 
        buffer.size()
    );

    // If the buffer was too small (rare for UTF-8 to UTF-16), we handle it,
    // though for standard UTF-8, output length <= input length in most cases.
    // For correctness with all possible UTF-8 strings:
    if (result.error == simdutf::result::error_buffer_too_small) {
        buffer.resize(result.required_size);
        result = simdutf::convert_utf8_to_utf16(
            reinterpret_cast<const char*>(input.data()), 
            input.size(), 
            buffer.data(), 
            buffer.size()
        );
    }

    uint64_t hash = 1469598103934665603ULL;
    const char16_t* data = buffer.data();
    size_t len = result.consumed_utf16;
    
    // Tight loop for FNV-1a checksum
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }

    return hash;
}