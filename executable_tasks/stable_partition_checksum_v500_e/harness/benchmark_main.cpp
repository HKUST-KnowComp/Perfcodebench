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

struct Input {
  std::vector<uint8_t> flags;
  std::vector<uint32_t> values;
};

Input make_input(std::size_t n) {
  Input out;
  out.flags.resize(n);
  out.values.resize(n);
  uint64_t state = 0xfeedfacecafebeefULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.flags[i] = static_cast<uint8_t>((state >> 27) & 1u);
    state = state * 2862933555777941757ULL + 3037000493ULL;
    out.values[i] = static_cast<uint32_t>((state >> 16) & 65535u);
  }
  return out;
}

uint64_t expected_hash(const std::vector<uint8_t>& flags, const std::vector<uint32_t>& values) {
  std::vector<uint32_t> out;
  out.reserve(values.size());
  for (std::size_t i = 0; i < values.size(); ++i) {
    if (flags[i]) {
      out.push_back(values[i]);
    }
  }
  for (std::size_t i = 0; i < values.size(); ++i) {
    if (!flags[i]) {
      out.push_back(values[i]);
    }
  }
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t v : out) {
    hash = mix(hash, v);
  }
  return hash;
}

}  // namespace

int main() {
  constexpr std::size_t kCount = 1u << 22;
  constexpr int kIters = 12;

  const Input input = make_input(kCount);
  const uint64_t expected = expected_hash(input.flags, input.values);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = stable_partition_hash(input.flags, input.values, kIters);
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
