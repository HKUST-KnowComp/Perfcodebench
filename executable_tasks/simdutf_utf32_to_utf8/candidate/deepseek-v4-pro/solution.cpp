#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstdint>

uint64_t transcode_checksum(const std::vector<char32_t>& input) {
    // Allocate worst-case output buffer (4 bytes per codepoint)
    std::vector<char> out(input.size() * 4);
    const char32_t* src = input.data();
    size_t src_len = input.size();
    char* dst = out.data();
    size_t actual_len = simdutf::utf32_to_utf8(src, src_len, dst);

    // Compute checksum exactly as in checksum_bytes over the first actual_len bytes
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* ptr = reinterpret_cast<unsigned char*>(dst);
    const unsigned char* end = ptr + actual_len;
    while (ptr < end) {
        hash ^= static_cast<uint64_t>(*ptr++);
        hash *= 1099511628211ULL;
    }
    return hash;
}