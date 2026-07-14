#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

std::vector<uint32_t> make_input(std::size_t n) {
  std::vector<uint32_t> values(n);
  uint64_t state = 0x3141592653589793ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    values[i] = static_cast<uint32_t>((state >> 20) & 65535u);
  }
  return values;
}

uint64_t expected_hash(const std::vector<uint32_t>& values, int window) {
  uint64_t hash = 1469598103934665603ULL;
  for (std::size_t i = 0; i + static_cast<std::size_t>(window) <= values.size(); ++i) {
    uint32_t mx = 0;
    for (int j = 0; j < window; ++j) {
      if (values[i + static_cast<std::size_t>(j)] > mx) {
        mx = values[i + static_cast<std::size_t>(j)];
      }
    }
    hash = mix(hash, mx);
  }
  return hash;
}

}  // namespace

int main() {
  constexpr std::size_t kCount = 1u << 20;
  constexpr int kWindow = 128;
  constexpr int kIters = 10;

  const std::vector<uint32_t> values = make_input(kCount);
  const uint64_t expected = expected_hash(values, kWindow);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = sliding_window_max_hash(values, kWindow, kIters);
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
