#include "interface.h"
#include "simdutf.h"
#include <string>
#include <memory>
#include <type_traits>

namespace {

template <typename T, typename = void>
struct CountExtractor {
    static size_t get(const T& res) { return static_cast<size_t>(res); }
};

template <typename T>
struct CountExtractor<T, std::void_t<decltype(std::declval<T>().count)>> {
    static size_t get(const T& res) { return res.count; }
};

uint64_t checksum_bytes(const char* data, size_t len) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < len; ++i) {
    hash ^= static_cast<uint64_t>(static_cast<unsigned char>(data[i]));
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

uint64_t decode_checksum(const std::string& input) {
  size_t max_len = (input.size() / 4) * 3 + 3;
  std::unique_ptr<char[]> out(new char[max_len]);
  
  auto res = simdutf::base64_to_binary(input.data(), input.size(), out.get());
  size_t actual_len = CountExtractor<decltype(res)>::get(res);
  
  return checksum_bytes(out.get(), actual_len);
}
