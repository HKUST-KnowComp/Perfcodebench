#include "interface.h"
#include "simdutf.h"
#include <vector>
#include <memory>

namespace {

// FNV-1a constants
static constexpr uint64_t FNV_OFFSET_BASIS = 1469598103934665603ULL;
static constexpr uint64_t FNV_PRIME = 1099511628211ULL;

inline uint64_t compute_checksum(const char16_t* data, size_t length) {
    uint64_t hash = FNV_OFFSET_BASIS;
    for (size_t i = 0; i < length; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= FNV_PRIME;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return FNV_OFFSET_BASIS;
    }

    // Calculate the required capacity for the UTF-16 output.
    // simdutf provides a fast way to count the number of code units.
    size_t utf16_length = simdutf::utf16_length_from_utf8(input.data(), input.size());
    
    // Use a stack buffer for small inputs or a heap-allocated buffer for large ones.
    // Given the 'xlarge input latency' hint, we use a vector for safety and simplicity,
    // but ensure we only allocate once.
    std::vector<char16_t> buffer(utf16_length);

    // Perform the SIMD-accelerated transcoding.
    // The input is guaranteed to be valid UTF-8 per task description.
    simdutf::convert_utf8_to_utf16le(input.data(), input.size(), buffer.data());

    // Compute the checksum over the resulting UTF-16 sequence.
    return compute_checksum(buffer.data(), utf16_length);
}