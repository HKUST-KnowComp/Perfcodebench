#include "interface.h"

#include <simdutf.h>
#include <string>
#include <cstdint>

namespace {

uint64_t checksum_bytes(const std::string& data) {
    uint64_t hash = 1469598103934665603ULL;
    const unsigned char* first = reinterpret_cast<const unsigned char*>(data.data());
    const unsigned char* last  = first + data.size();
    while (first != last) {
        hash ^= static_cast<uint64_t>(*first++);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t transcode_checksum(const std::u16string& input) {
    const std::string utf8 = simdutf::convert_utf16_to_utf8(
        reinterpret_cast<const char16_t*>(input.data()),
        input.size()
    );
    return checksum_bytes(utf8);
}
