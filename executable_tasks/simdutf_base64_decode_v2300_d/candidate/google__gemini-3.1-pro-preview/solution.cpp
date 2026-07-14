#include "interface.h"
#include <string>
#include <memory>
#include <type_traits>
#include <simdutf.h>

namespace {

uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  size_t i = 0;
  for (; i + 7 < len; i += 8) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i+1]));
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i+2]));
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i+3]));
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i+4]));
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i+5]));
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i+6]));
    hash *= 1099511628211ULL;
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i+7]));
    hash *= 1099511628211ULL;
  }
  for (; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

template <typename T>
size_t get_count(const T& res) {
    if constexpr (std::is_same_v<T, size_t>) {
        return res;
    } else {
        return res.count;
    }
}

}  // namespace

uint64_t decode_checksum(const std::string& input) {
  size_t max_len = (input.size() / 4) * 3 + 3;
  std::unique_ptr<char[]> out(new char[max_len]);
  auto res = simdutf::base64_to_binary(input.data(), input.size(), out.get());
  size_t actual_len = get_count(res);
  return checksum_bytes(out.get(), actual_len);
}
