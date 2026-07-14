#include "interface.h"

#include "zstd.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>

namespace {

std::string make_input(std::size_t size) {
  std::mt19937 rng(424242);
  std::uniform_int_distribution<int> dist(0, 7);
  std::string s;
  s.resize(size);
  for (std::size_t i = 0; i < size; ++i) {
    s[i] = static_cast<char>('a' + dist(rng));
  }
  return s;
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
  const std::string input = make_input(300000);
  std::string compressed;
  compressed.resize(ZSTD_compressBound(input.size()));
  const size_t n = ZSTD_compress(
      compressed.data(), compressed.size(), input.data(), input.size(), 1);
  compressed.resize(n);
  const uint64_t expected = checksum_bytes(input);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = decompress_checksum(compressed, input.size());
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
