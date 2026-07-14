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

std::vector<uint32_t> make_input(std::size_t n, uint32_t bins) {
  std::vector<uint32_t> values(n);
  uint64_t state = 0x9e3779b97f4a7c15ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    values[i] = static_cast<uint32_t>(state & static_cast<uint64_t>(bins - 1u));
  }
  return values;
}

uint64_t expected_hash(const std::vector<uint32_t>& values, uint32_t bins) {
  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);
  for (uint32_t v : values) {
    ++counts[static_cast<std::size_t>(v)];
  }
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t c : counts) {
    hash = mix(hash, c);
  }
  return hash;
}

}  // namespace

int main() {
  constexpr std::size_t kCount = 1u << 23;
  constexpr uint32_t kBins = 4096;
  constexpr int kIters = 12;

  const std::vector<uint32_t> values = make_input(kCount, kBins);
  const uint64_t expected = expected_hash(values, kBins);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = histogram_checksum(values, kBins, kIters);
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
