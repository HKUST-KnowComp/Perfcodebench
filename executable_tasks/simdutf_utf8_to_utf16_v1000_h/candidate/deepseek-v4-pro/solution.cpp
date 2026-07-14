#include "interface.h"
#include <simdutf.h>
#include <vector>

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        // initial hash for empty output
        return 1469598103934665603ULL;
    }
    // maximum possible number of UTF-16 code units is input.size() (all ASCII)
    std::vector<char16_t> out(input.size());
    // SIMD-accelerated conversion to UTF-16LE
    size_t out_len = simdutf::convert_utf8_to_utf16le(input.data(), input.size(), out.data());
    out.resize(out_len);  // optional but safe

    // compute the required checksum over the exact output sequence
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < out_len; ++i) {
        hash ^= static_cast<uint64_t>(out[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}