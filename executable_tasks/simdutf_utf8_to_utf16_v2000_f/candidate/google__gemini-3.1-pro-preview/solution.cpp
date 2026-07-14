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
    const char16_t* end = data + length;
    for (; data < end; ++data) {
        hash ^= static_cast<uint64_t>(*data);
        hash *= FNV_PRIME;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return FNV_OFFSET_BASIS;
    }

    // Calculate the exact number of UTF-16 code units needed.
    // simdutf provides a fast way to count this.
    size_t utf16_length = simdutf::utf16_length_from_utf8(input.data(), input.size());

    // Use a stack buffer for small strings to avoid heap allocation overhead.
    // 4096 bytes allows for 2048 char16_t units.
    char16_t stack_buffer[1024];
    char16_t* buffer_ptr = nullptr;
    std::unique_ptr<char16_t[]> heap_buffer;

    if (utf16_length <= 1024) {
        buffer_ptr = stack_buffer;
    } else {
        heap_buffer.reset(new char16_t[utf16_length]);
        buffer_ptr = heap_buffer.get();
    }

    // Perform the conversion using SIMD-accelerated simdutf.
    // The task guarantees valid UTF-8, so we use the convert function.
    size_t written = simdutf::convert_utf8_to_utf16(input.data(), input.size(), buffer_ptr);

    // Compute the checksum over the resulting UTF-16 sequence.
    return compute_checksum(buffer_ptr, written);
}