#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

struct Input {
  std::vector<uint32_t> keys;
  std::vector<uint8_t> flags;
  std::vector<uint32_t> payloads;
};

Input make_input(std::size_t n) {
  Input out;
  out.keys.resize(n);
  out.flags.resize(n);
  out.payloads.resize(n);
  uint64_t state = 0x123456789abcdef0ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.keys[i] = static_cast<uint32_t>((state >> 16) & 8191u);
    state = state * 2862933555777941757ULL + 3037000493ULL;
    out.flags[i] = static_cast<uint8_t>(((state >> 27) & 3u) != 0u);
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.payloads[i] = static_cast<uint32_t>((state >> 20) & 65535u);
  }
  return out;
}

uint64_t expected_sum(
    const std::vector<uint32_t>& keys,
    const std::vector<uint8_t>& flags,
    const std::vector<uint32_t>& payloads,
    uint32_t threshold) {
  uint64_t sum = 0;
  for (std::size_t i = 0; i < keys.size(); ++i) {
    if (flags[i] && keys[i] < threshold && ((keys[i] & 7u) == 0u)) {
      sum += static_cast<uint64_t>(payloads[i]);
    }
  }
  return sum;
}

}  // namespace

int main() {
  constexpr std::size_t kCount = 1u << 24;
  constexpr uint32_t kThreshold = 4096;
  constexpr int kIters = 12;

  const Input input = make_input(kCount);
  const uint64_t expected =
      expected_sum(input.keys, input.flags, input.payloads, kThreshold);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual =
      selection_gather_sum(input.keys, input.flags, input.payloads, kThreshold, kIters);
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
