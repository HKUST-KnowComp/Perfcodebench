#include "interface.h"

#include <simdutf.h>

#include <cstdint>
#include <string>

namespace {

static inline uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<unsigned char>(data[i]);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t decode_checksum(const std::string& input) {
  // Fast path using simdutf's optimized base64 decoder.
  // The task guarantees standard base64 input; we preserve correctness by
  // decoding the exact bytes and hashing only the produced output length.
  const size_t max_len = simdutf::maximal_binary_length_from_base64(input.data(), input.size());
  std::string decoded;
  decoded.resize(max_len);

  simdutf::result r = simdutf::base64_to_binary(
      input.data(),
      input.size(),
      decoded.data());

  // For valid standard base64, decoding succeeds. In the unlikely event of an
  // error, return the checksum of the successfully produced prefix to avoid
  // undefined behavior while remaining deterministic.
  decoded.resize(r.count);
  return checksum_bytes(decoded.data(), decoded.size());
}
