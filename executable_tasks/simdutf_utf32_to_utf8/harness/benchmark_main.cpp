#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace {

std::vector<char32_t> make_input(std::size_t count) {
  std::vector<char32_t> input;
  input.reserve(count);
  uint64_t state = 0x1122334455667788ULL;
  for (std::size_t i = 0; i < count; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    switch (state % 5ULL) {
      case 0: input.push_back(static_cast<char32_t>(0x20U + (state % 0x5FU))); break;
      case 1: input.push_back(static_cast<char32_t>(0x00C0U + (state % 0x02FFU))); break;
      case 2: input.push_back(static_cast<char32_t>(0x0800U + (state % 0x07FFU))); break;
      case 3: input.push_back(static_cast<char32_t>(0x4E00U + (state % 0x05FFU))); break;
      default: input.push_back(static_cast<char32_t>(0x1F300U + (state % 0x01FFU))); break;
    }
  }
  return input;
}

void append_utf8(std::string& out, uint32_t cp) {
  if (cp <= 0x7F) {
    out.push_back(static_cast<char>(cp));
  } else if (cp <= 0x7FF) {
    out.push_back(static_cast<char>(0xC0U | (cp >> 6)));
    out.push_back(static_cast<char>(0x80U | (cp & 0x3FU)));
  } else if (cp <= 0xFFFF) {
    out.push_back(static_cast<char>(0xE0U | (cp >> 12)));
    out.push_back(static_cast<char>(0x80U | ((cp >> 6) & 0x3FU)));
    out.push_back(static_cast<char>(0x80U | (cp & 0x3FU)));
  } else {
    out.push_back(static_cast<char>(0xF0U | (cp >> 18)));
    out.push_back(static_cast<char>(0x80U | ((cp >> 12) & 0x3FU)));
    out.push_back(static_cast<char>(0x80U | ((cp >> 6) & 0x3FU)));
    out.push_back(static_cast<char>(0x80U | (cp & 0x3FU)));
  }
}

uint64_t checksum_bytes(const std::string& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t expected_checksum(const std::vector<char32_t>& input) {
  std::string out;
  out.reserve(input.size() * 4);
  for (char32_t cp : input) {
    append_utf8(out, static_cast<uint32_t>(cp));
  }
  return checksum_bytes(out);
}

}  // namespace

int main() {
  const std::vector<char32_t> input = make_input(260000);
  const uint64_t expected = expected_checksum(input);

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
