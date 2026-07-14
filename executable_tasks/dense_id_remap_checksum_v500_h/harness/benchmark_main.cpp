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
  std::vector<uint32_t> keys;
  std::vector<uint32_t> values;
  std::vector<uint32_t> probes;
};

Input make_input(std::size_t domain, std::size_t rows) {
  Input out;
  out.keys.resize(domain);
  out.values.resize(domain);
  out.probes.resize(rows);
  for (std::size_t i = 0; i < domain; ++i) {
    out.keys[i] = static_cast<uint32_t>(i);
    out.values[i] = static_cast<uint32_t>(((i * 1103515245u) + 12345u) & 0xffffu);
  }
  uint64_t state = 0x123456789abcdef0ULL;
  for (std::size_t i = 0; i < rows; ++i) {
    state = state * 2862933555777941757ULL + 3037000493ULL;
    out.probes[i] = static_cast<uint32_t>(state & static_cast<uint64_t>(domain - 1u));
  }
  return out;
}

uint64_t expected_checksum(
    const std::vector<uint32_t>& values,
    const std::vector<uint32_t>& probes) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t probe : probes) {
    hash = mix(hash, values[static_cast<std::size_t>(probe)]);
  }
  return hash;
}

}  // namespace

int main() {
  constexpr std::size_t kDomain = 1u << 19;
  constexpr std::size_t kRows = 1u << 21;
  constexpr int kIters = 12;

  const Input input = make_input(kDomain, kRows);
  const uint64_t expected = expected_checksum(input.values, input.probes);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = remap_checksum(input.keys, input.values, input.probes, kIters);
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
