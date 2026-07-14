#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

std::vector<uint32_t> make_keys(std::size_t n, uint32_t groups) {
  std::vector<uint32_t> out(n);
  uint64_t state = 0x123456789abcdef0ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out[i] = static_cast<uint32_t>(state % static_cast<uint64_t>(groups));
  }
  return out;
}

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

uint64_t expected_checksum(const std::vector<uint32_t>& keys, uint32_t groups) {
  std::vector<uint32_t> counts(static_cast<std::size_t>(groups), 0);
  for (uint32_t key : keys) {
    ++counts[static_cast<std::size_t>(key)];
  }
  std::vector<uint32_t> offsets(static_cast<std::size_t>(groups) + 1, 0);
  for (uint32_t g = 0; g < groups; ++g) {
    offsets[static_cast<std::size_t>(g + 1)] =
        offsets[static_cast<std::size_t>(g)] + counts[static_cast<std::size_t>(g)];
  }
  std::vector<uint32_t> next = offsets;
  std::vector<uint32_t> rows(keys.size(), 0);
  for (std::size_t i = 0; i < keys.size(); ++i) {
    const uint32_t key = keys[i];
    rows[static_cast<std::size_t>(next[static_cast<std::size_t>(key)]++)] = static_cast<uint32_t>(i);
  }
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t v : offsets) {
    hash = mix(hash, v);
  }
  for (uint32_t v : rows) {
    hash = mix(hash, v);
  }
  return hash;
}

}  // namespace

int main() {
  constexpr std::size_t kCount = 1u << 22;
  constexpr uint32_t kGroups = 256;
  constexpr int kIters = 12;

  const std::vector<uint32_t> keys = make_keys(kCount, kGroups);
  const uint64_t expected = expected_checksum(keys, kGroups);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = partition_scatter_checksum(keys, kGroups, kIters);
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
