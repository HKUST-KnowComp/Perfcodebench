#include "interface.h"
#include "simdutf.h"
#include <string>
#include <vector>

namespace {

// FNV-1a 64-bit hash implementation
inline uint64_t checksum_bytes(const std::string& data) {
    uint64_t hash = 1469598103934665603ULL;
    for (unsigned char ch : data) {
        hash ^= static_cast<uint64_t>(ch);
        hash *= 1099511628211ULL;
    }
    return hash;
}

} // namespace

uint64_t decode_checksum(const std::string& input) {
    if (input.empty()) {
        return 1469598103934665603ULL;
    }

    // simdutf::base64::unpack is highly optimized for various architectures.
    // It handles padding and returns a std::string containing the decoded bytes.
    std::string decoded;
    auto status = simdutf::base64::unpack(input.data(), input.size(), decoded);
    
    // If decoding fails (though task implies valid input), we fallback to a safe state.
    // However, for performance in a competitive context, we assume valid base64.
    if (status != simdutf::outcome::success) {
        // This part is technically unreachable given the task constraints
        return 0;
    }

    return checksum_bytes(decoded);
}
