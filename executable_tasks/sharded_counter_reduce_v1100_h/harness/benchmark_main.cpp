#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

struct Input {
  std::vector<uint32_t> keys;
  std::vector<uint32_t> values;
};

Input make_input(std::size_t n, uint32_t key_space) {
  Input out;
  out.keys.resize(n);
  out.values.resize(n);
  uint64_t state = 0x13579bdf2468ace0ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.keys[i] = static_cast<uint32_t>(state % static_cast<uint64_t>(key_space));
    state = state * 2862933555777941757ULL + 3037000493ULL;
    out.values[i] = static_cast<uint32_t>((state & 15u) + 1u);
  }
  return out;
}

uint64_t checksum_u64(const std::vector<uint64_t>& counts) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint64_t v : counts) {
    hash ^= v;
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t expected_checksum(const std::vector<uint32_t>& keys, const std::vector<uint32_t>& values, uint32_t key_space) {
  std::vector<uint64_t> counts(static_cast<std::size_t>(key_space), 0);
  for (std::size_t i = 0; i < keys.size(); ++i) {
    counts[static_cast<std::size_t>(keys[i])] += static_cast<uint64_t>(values[i]);
  }
  return checksum_u64(counts);
}

}  // namespace

int main() {
  constexpr uint32_t kKeySpace = 1u << 20;
  constexpr int kThreads = 8;
  constexpr int kIters = 2;
  const Input input = make_input(1u << 22, kKeySpace);
  const uint64_t expected = expected_checksum(input.keys, input.values, kKeySpace);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual =
      sharded_counter_checksum(input.keys, input.values, kKeySpace, kThreads, kIters);
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
