#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

std::vector<uint8_t> make_input(std::size_t n) {
  std::vector<uint8_t> out(n);
  uint64_t state = 0x0fedcba987654321ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 2862933555777941757ULL + 3037000493ULL;
    if ((i & 15u) == 0u) {
      out[i] = static_cast<uint8_t>((state >> 24) & 0xff);
    } else {
      out[i] = static_cast<uint8_t>((state >> 24) & 31u);
    }
  }
  return out;
}

uint64_t checksum_u32(const std::vector<uint32_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t v : data) {
    hash ^= static_cast<uint64_t>(v);
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t expected_checksum(const std::vector<uint8_t>& input, int bins) {
  std::vector<uint32_t> counts(static_cast<std::size_t>(bins), 0);
  for (uint8_t value : input) {
    ++counts[static_cast<std::size_t>(value)];
  }
  return checksum_u32(counts);
}

}  // namespace

int main() {
  constexpr std::size_t kCount = 1u << 26;
  constexpr int kBins = 256;
  constexpr int kIters = 4;

  const std::vector<uint8_t> input = make_input(kCount);
  const uint64_t expected = expected_checksum(input, kBins);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = histogram_checksum(input, kBins, kIters);
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
