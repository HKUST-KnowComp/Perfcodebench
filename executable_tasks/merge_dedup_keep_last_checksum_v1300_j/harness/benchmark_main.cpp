#include "interface.h"

#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <utility>
#include <vector>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

struct Input {
  std::vector<uint32_t> left_keys;
  std::vector<uint32_t> left_values;
  std::vector<uint32_t> right_keys;
  std::vector<uint32_t> right_values;
};

Input make_input(std::size_t n) {
  Input out;
  out.left_keys.resize(n);
  out.left_values.resize(n);
  out.right_keys.resize(n);
  out.right_values.resize(n);
  uint32_t key = 0;
  for (std::size_t i = 0; i < n; ++i) {
    key += static_cast<uint32_t>((i & 3u) + 1u);
    out.left_keys[i] = key;
    out.left_values[i] = static_cast<uint32_t>(((i * 13u) + 7u) & 4095u);
  }
  key = 0;
  for (std::size_t i = 0; i < n; ++i) {
    key += static_cast<uint32_t>((i & 7u) + 1u);
    out.right_keys[i] = key;
    out.right_values[i] = static_cast<uint32_t>(((i * 17u) + 11u) & 4095u);
  }
  return out;
}

uint64_t expected_hash(
    const std::vector<uint32_t>& left_keys,
    const std::vector<uint32_t>& left_values,
    const std::vector<uint32_t>& right_keys,
    const std::vector<uint32_t>& right_values) {
  std::vector<std::pair<uint32_t, uint32_t>> merged;
  merged.reserve(left_keys.size() + right_keys.size());
  for (std::size_t i = 0; i < left_keys.size(); ++i) {
    merged.emplace_back(left_keys[i], left_values[i]);
  }
  for (std::size_t i = 0; i < right_keys.size(); ++i) {
    merged.emplace_back(right_keys[i], right_values[i]);
  }
  std::stable_sort(
      merged.begin(),
      merged.end(),
      [](const auto& a, const auto& b) { return a.first < b.first; });
  std::vector<std::pair<uint32_t, uint32_t>> out;
  out.reserve(merged.size());
  for (const auto& kv : merged) {
    if (!out.empty() && out.back().first == kv.first) {
      out.back().second = kv.second;
    } else {
      out.push_back(kv);
    }
  }
  uint64_t hash = 1469598103934665603ULL;
  for (const auto& kv : out) {
    hash = mix(hash, kv.first);
    hash = mix(hash, kv.second);
  }
  return hash;
}

}  // namespace

int main() {
  constexpr std::size_t kCount = 1u << 23;
  constexpr int kIters = 2;

  const Input input = make_input(kCount);
  const uint64_t expected =
      expected_hash(input.left_keys, input.left_values, input.right_keys, input.right_values);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = merge_keep_last_hash(
      input.left_keys,
      input.left_values,
      input.right_keys,
      input.right_values,
      kIters);
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
