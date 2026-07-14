#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

std::vector<uint16_t> make_values(std::size_t n) {
  std::vector<uint16_t> values(n);
  uint64_t state = 0x9e3779b97f4a7c15ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    values[i] = static_cast<uint16_t>((state >> 16) & 4095u);
  }
  return values;
}

std::vector<uint8_t> pack_values(const std::vector<uint16_t>& values) {
  std::vector<uint8_t> packed;
  packed.reserve((values.size() / 2u) * 3u);
  for (std::size_t i = 0; i < values.size(); i += 2u) {
    const uint16_t a = values[i];
    const uint16_t b = values[i + 1u];
    packed.push_back(static_cast<uint8_t>(a & 0xffu));
    packed.push_back(static_cast<uint8_t>(((a >> 8u) & 0x0fu) | ((b & 0x0fu) << 4u)));
    packed.push_back(static_cast<uint8_t>((b >> 4u) & 0xffu));
  }
  return packed;
}

uint64_t expected_sum(const std::vector<uint16_t>& values, uint16_t threshold) {
  uint64_t sum = 0;
  for (uint16_t v : values) {
    if (v > threshold) {
      sum += static_cast<uint64_t>(v);
    }
  }
  return sum;
}

}  // namespace

int main() {
  constexpr std::size_t kCount = 1u << 23;
  constexpr uint16_t kThreshold = 2048;
  constexpr int kIters = 16;

  const std::vector<uint16_t> values = make_values(kCount);
  const std::vector<uint8_t> packed = pack_values(values);
  const uint64_t expected = expected_sum(values, kThreshold);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = packed12_filter_sum(packed, kThreshold, kIters);
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
