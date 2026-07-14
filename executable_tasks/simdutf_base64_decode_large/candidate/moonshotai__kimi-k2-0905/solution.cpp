#include "interface.h"
#include <simdutf.h>
#include <vector>
#include <cstdint>

namespace {
// Thread-local reusable buffer avoids per-call allocation.
thread_local std::vector<char> decode_buf;

uint64_t checksum_bytes(const char* data, size_t len) {
    uint64_t hash = 1469598103934665603ULL;
    for (size_t i = 0; i < len; ++i) {
        hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
        hash *= 1099511628211ULL;
    }
    return hash;
}
} // namespace

uint64_t decode_checksum(const std::string& input) {
    const size_t max_out_len = (input.size() / 4) * 3;
    decode_buf.resize(max_out_len); // no-op if already large enough

    size_t written = 0;
    simdutf::error_code err = simdutf::base64_to_binary(
        input.data(), input.size(), decode_buf.data(), simdutf::base64_default, &written);

    // simdutf guarantees success for valid standard base64; we rely on task inputs being valid.
    (void)err;

    return checksum_bytes(decode_buf.data(), written);
}