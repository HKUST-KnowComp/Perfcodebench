#include "interface.h"
#include "simdjson.h"

/**
 * The simdjson library provides a highly optimized validate_utf8 function.
 * It uses SIMD instructions to process chunks of data, which is significantly
 * faster than manual scalar validation.
 */
std::size_t count_valid_strings(const std::vector<std::string>& inputs) {
  std::size_t count = 0;
  for (const auto& s : inputs) {
    // simdjson::validate_utf8 returns true if the input is valid UTF-8.
    // It is designed to be extremely fast and handles various CPU architectures.
    if (simdjson::validate_utf8(s.data(), s.size())) {
      count++;
    }
  }
  return count;
}