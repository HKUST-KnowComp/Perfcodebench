#include "interface.h"

#include "xxhash.h"

#include <algorithm>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>

namespace {

constexpr std::size_t kChunkSize = 1 << 10;

std::string make_input(std::size_t size) {
  std::mt19937 rng(20260429);
  std::uniform_int_distribution<int> dist(0, 255);
  std::string s;
  s.resize(size);
  for (std::size_t i = 0; i < size; ++i) {
    s[i] = static_cast<char>(dist(rng));
  }
  return s;
}

uint64_t expected_hash(const std::string& input) {
  uint64_t acc = 0;
  for (std::size_t offset = 0; offset < input.size(); offset += kChunkSize) {
    const std::size_t take = std::min(kChunkSize, input.size() - offset);
    acc ^= static_cast<uint64_t>(XXH64(input.data() + offset, take, 0));
    acc = (acc << 1) | (acc >> 63);
  }
  return acc;
}

}  // namespace

int main() {
  const std::string input = make_input(1 << 25);
  const uint64_t expected = expected_hash(input);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = chunked_hash(input);
  const auto end = std::chrono::steady_clock::now();

  const bool ok = actual == expected;
  const long long elapsed_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  std::cout << std::boolalpha;
  std::cout << "{"
            << "\"ok\":" << ok << ","
            << "\"elapsed_ns\":" << elapsed_ns << ","
            << "\"expected_hash\":" << expected << ","
            << "\"actual_hash\":" << actual << "}\n";
  return ok ? 0 : 1;
}
