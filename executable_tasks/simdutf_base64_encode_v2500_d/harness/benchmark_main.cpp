#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>

namespace {

constexpr char kTable[] =
    "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

std::string make_input(std::size_t size) {
  std::string input;
  input.resize(size);
  uint64_t state = 0x9e3779b97f4a7c15ULL;
  for (std::size_t i = 0; i < size; ++i) {
    state = state * 2862933555777941757ULL + 3037000493ULL;
    input[i] = static_cast<char>(state >> 56);
  }
  return input;
}

std::string scalar_base64(const std::string& input) {
  std::string out;
  out.reserve(((input.size() + 2) / 3) * 4);
  for (std::size_t i = 0; i < input.size(); i += 3) {
    const uint32_t b0 = static_cast<unsigned char>(input[i]);
    const uint32_t b1 = (i + 1 < input.size()) ? static_cast<unsigned char>(input[i + 1]) : 0U;
    const uint32_t b2 = (i + 2 < input.size()) ? static_cast<unsigned char>(input[i + 2]) : 0U;
    const uint32_t triple = (b0 << 16) | (b1 << 8) | b2;
    out.push_back(kTable[(triple >> 18) & 0x3FU]);
    out.push_back(kTable[(triple >> 12) & 0x3FU]);
    out.push_back((i + 1 < input.size()) ? kTable[(triple >> 6) & 0x3FU] : '=');
    out.push_back((i + 2 < input.size()) ? kTable[triple & 0x3FU] : '=');
  }
  return out;
}

uint64_t checksum_bytes(const std::string& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (unsigned char ch : data) {
    hash ^= static_cast<uint64_t>(ch);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

int main() {
    const std::string input = make_input(1 << 24);
  const uint64_t expected = checksum_bytes(scalar_base64(input));

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = encode_checksum(input);
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
