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
  std::vector<uint32_t> groups;
  std::vector<uint32_t> values;
};

Input make_input(std::size_t n, uint32_t group_count) {
  Input out;
  out.groups.resize(n);
  out.values.resize(n);
  uint64_t state = 0x123456789abcdef0ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.groups[i] = static_cast<uint32_t>(state & static_cast<uint64_t>(group_count - 1u));
    state = state * 2862933555777941757ULL + 3037000493ULL;
    out.values[i] = static_cast<uint32_t>((state >> 16) & 65535u);
  }
  return out;
}

uint64_t expected_hash(
    const std::vector<uint32_t>& groups,
    const std::vector<uint32_t>& values,
    uint32_t group_count) {
  std::vector<uint32_t> best(static_cast<std::size_t>(group_count), 0);
  for (std::size_t i = 0; i < groups.size(); ++i) {
    uint32_t& slot = best[static_cast<std::size_t>(groups[i])];
    if (values[i] > slot) {
      slot = values[i];
    }
  }
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t v : best) {
    hash = mix(hash, v);
  }
  return hash;
}

}  // namespace

int main() {
  constexpr std::size_t kCount = 1u << 25;
  constexpr uint32_t kGroups = 8192;
  constexpr int kIters = 8;

  const Input input = make_input(kCount, kGroups);
  const uint64_t expected = expected_hash(input.groups, input.values, kGroups);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = top1_group_hash(input.groups, input.values, kGroups, kIters);
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
