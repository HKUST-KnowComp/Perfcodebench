#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstdint>

namespace {

inline uint64_t update_fnv1a(uint64_t hash, uint8_t byte) noexcept {
    constexpr uint64_t FNV_PRIME = 1099511628211ULL;
    hash ^= static_cast<uint64_t>(byte);
    hash *= FNV_PRIME;
    return hash;
}

} // anonymous namespace

uint64_t transcode_checksum(const std::vector<char32_t>& input) {
    if (input.empty()) return 1469598103934665603ULL; // FNV offset_basis

    // simdutf can convert directly from UTF-32 to UTF-8
    const size_t max_utf8_bytes = input.size() * 4;
    thread_local std::vector<char> buffer;
    buffer.resize(max_utf8_bytes);

    const auto* src = reinterpret_cast<const char32_t*>(input.data());
    char* dst = buffer.data();
    size_t utf8_len = simdutf::convert_utf32_to_utf8(src, input.size(), dst);

    // Compute FNV-1a checksum over the exact UTF-8 byte sequence
    uint64_t hash = 1469598103934665603ULL; // FNV offset_basis
    const unsigned char* bytes = reinterpret_cast<const unsigned char*>(dst);
    for (size_t i = 0; i < utf8_len; ++i) {
        hash = update_fnv1a(hash, bytes[i]);
    }
    return hash;
}