#include "interface.h"
#include "simdutf.h"
#include <vector>

namespace {

uint64_t compute_checksum(const char16_t* data, size_t length) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < length; ++i) {
        hash ^= static_cast<uint64_t>(data[i]);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // simdutf::utf8_to_utf16 requires a destination buffer.
    // The maximum size for UTF-16 is the same as UTF-8 size (for ASCII) 
    // or slightly more in specific edge cases, but generally input.size() is a safe upper bound 
    // for the number of char16_t units since UTF-8 is more compact than UTF-16 for most characters
    // except for the 4-byte sequences which become 2 UTF-16 units (surrogates).
    // A safe allocation is input.size() * 2 if we want to be absolutely sure, 
    // but for valid UTF-8, the number of UTF-16 code units is at most input.size().
    
    std::vector<char16_t> buffer(input.size());
    
    simdutf::utf8_to_utf16_result result = simdutf::utf8_to_utf16(
        reinterpret_cast<const char16_t*>(const_cast<char*>(input.data())),
        input.size(),
        buffer.data(),
        buffer.size()
    );

    // The result.length is the number of char16_t written to the buffer.
    return compute_checksum(buffer.data(), result.length);
}