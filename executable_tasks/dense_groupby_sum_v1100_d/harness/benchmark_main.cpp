#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

void make_input(std::vector<uint32_t>& keys, std::vector<uint32_t>& values, std::size_t n, int groups) {
  keys.resize(n);
  values.resize(n);
  uint64_t state = 0x0fedcba987654321ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 2862933555777941757ULL + 3037000493ULL;
    keys[i] = static_cast<uint32_t>(state % static_cast<uint64_t>(groups));
    state = state * 2862933555777941757ULL + 3037000493ULL;
    values[i] = static_cast<uint32_t>(state & 1023u);
  }
}

uint64_t checksum_u64(const std::vector<uint64_t>& data) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint64_t v : data) {
    hash ^= v;
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t expected_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, int groups) {
  std::vector<uint64_t> agg(static_cast<std::size_t>(groups), 0);
  for (std::size_t i = 0; i < keys.size(); ++i) {
    agg[static_cast<std::size_t>(keys[i])] += static_cast<uint64_t>(values[i]);
  }
  return checksum_u64(agg);
}

}  // namespace

int main() {
  constexpr std::size_t kCount = 1u << 25;
  constexpr int kGroups = 4096;
  constexpr int kIters = 6;

  std::vector<uint32_t> keys;
  std::vector<uint32_t> values;
  make_input(keys, values, kCount, kGroups);
  const uint64_t expected = expected_checksum(keys, values, kGroups);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = groupby_checksum(keys, values, kGroups, kIters);
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
