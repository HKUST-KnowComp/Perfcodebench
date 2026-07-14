#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

std::vector<uint8_t> make_input(std::size_t bytes) {
  std::vector<uint8_t> packed(bytes);
  uint64_t state = 0xabcdef0123456789ULL;
  for (std::size_t i = 0; i < bytes; ++i) {
    state = state * 2862933555777941757ULL + 3037000493ULL;
    packed[i] = static_cast<uint8_t>(state & 0xffu);
  }
  return packed;
}

uint64_t expected_sum(const std::vector<uint8_t>& packed, uint8_t threshold) {
  uint64_t sum = 0;
  for (uint8_t byte : packed) {
    const uint8_t lo = static_cast<uint8_t>(byte & 15u);
    const uint8_t hi = static_cast<uint8_t>(byte >> 4u);
    if (lo > threshold) {
      sum += static_cast<uint64_t>(lo);
    }
    if (hi > threshold) {
      sum += static_cast<uint64_t>(hi);
    }
  }
  return sum;
}

}  // namespace

int main() {
  constexpr std::size_t kBytes = 1u << 22;
  constexpr uint8_t kThreshold = 3;
  constexpr int kIters = 24;

  const std::vector<uint8_t> packed = make_input(kBytes);
  const uint64_t expected = expected_sum(packed, kThreshold);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = nibble_filter_sum(packed, kThreshold, kIters);
  const auto end = std::chrono::steady_clock::now();

  const bool ok = actual == expected;
  const long long elapsed_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  std::cout << std::boolalpha;
  std::cout << "{"
            << "\"ok\":" << ok << ","
            << "\"elapsed_ns\":" << elapsed_ns << ","
            << "\"expected_sum\":" << expected << ","
            << "\"actual_sum\":" << actual << "}\n";
  return ok ? 0 : 1;
}
