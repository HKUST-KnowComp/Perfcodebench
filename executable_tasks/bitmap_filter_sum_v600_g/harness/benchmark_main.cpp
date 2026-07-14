#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

struct Input {
  std::vector<uint32_t> enabled_keys;
  std::vector<uint32_t> probe_keys;
  std::vector<uint32_t> values;
};

Input make_input(uint32_t key_space, std::size_t probes) {
  Input out;
  out.enabled_keys.reserve(static_cast<std::size_t>(key_space / 2));
  std::vector<uint8_t> used(static_cast<std::size_t>(key_space), 0);
  uint64_t state = 0x9e3779b97f4a7c15ULL;
  while (out.enabled_keys.size() < static_cast<std::size_t>(key_space / 2)) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    const uint32_t key = static_cast<uint32_t>(state % static_cast<uint64_t>(key_space));
    if (!used[static_cast<std::size_t>(key)]) {
      used[static_cast<std::size_t>(key)] = 1;
      out.enabled_keys.push_back(key);
    }
  }
  out.probe_keys.resize(probes);
  out.values.resize(probes);
  for (std::size_t i = 0; i < probes; ++i) {
    state = state * 2862933555777941757ULL + 3037000493ULL;
    out.probe_keys[i] = static_cast<uint32_t>(state % static_cast<uint64_t>(key_space));
    state = state * 2862933555777941757ULL + 3037000493ULL;
    out.values[i] = static_cast<uint32_t>((state & 1023u) + 1u);
  }
  return out;
}

uint64_t expected_sum(
    const std::vector<uint32_t>& enabled_keys,
    const std::vector<uint32_t>& probe_keys,
    const std::vector<uint32_t>& values,
    uint32_t key_space) {
  std::vector<uint8_t> enabled(static_cast<std::size_t>(key_space), 0);
  for (uint32_t key : enabled_keys) {
    enabled[static_cast<std::size_t>(key)] = 1;
  }
  uint64_t sum = 0;
  for (std::size_t i = 0; i < probe_keys.size(); ++i) {
    if (enabled[static_cast<std::size_t>(probe_keys[i])]) {
      sum += static_cast<uint64_t>(values[i]);
    }
  }
  return sum;
}

}  // namespace

int main() {
  constexpr uint32_t kKeySpace = 1u << 18;
  const Input input = make_input(kKeySpace, 1u << 23);
  constexpr int kIters = 20;
  const uint64_t expected =
      expected_sum(input.enabled_keys, input.probe_keys, input.values, kKeySpace);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = bitmap_filter_sum(
      input.enabled_keys, input.probe_keys, input.values, kKeySpace, kIters);
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
