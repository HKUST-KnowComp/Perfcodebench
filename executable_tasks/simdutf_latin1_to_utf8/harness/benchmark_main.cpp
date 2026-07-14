#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>

namespace {

std::string make_input(std::size_t size) {
  std::string input;
  input.resize(size);
  uint64_t state = 0xfedcba9876543210ULL;
  for (std::size_t i = 0; i < size; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    input[i] = static_cast<char>(state >> 56);
  }
  return input;
}

uint64_t checksum_bytes(const std::string& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

std::string latin1_to_utf8_scalar(const std::string& input) {
  std::string out;
  out.reserve(input.size() * 2);
  for (unsigned char ch : input) {
    if (ch <= 0x7FU) {
      out.push_back(static_cast<char>(ch));
    } else {
      out.push_back(static_cast<char>(0xC0U | (ch >> 6)));
      out.push_back(static_cast<char>(0x80U | (ch & 0x3FU)));
    }
  }
  return out;
}

}  // namespace

int main() {
  const std::string input = make_input(1 << 20);
  const uint64_t expected = checksum_bytes(latin1_to_utf8_scalar(input));

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = transcode_checksum(input);
  const auto end = std::chrono::steady_clock::now();

  const bool ok = actual == expected;
  const long long elapsed_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  std::cout << std::boolalpha;
  std::cout << "{"
            << "\"ok\":" << ok << ","
            << "\"elapsed_ns\":" << elapsed_ns << ","
            << "\"expected_checksum\":" << expected << ","
            << "\"actual_checksum\":" << actual << "}\n";
  return ok ? 0 : 1;
}
