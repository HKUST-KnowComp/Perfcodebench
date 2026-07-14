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
  std::vector<uint32_t> indices;
  std::vector<uint32_t> values;
};

Input make_input(std::size_t n, uint32_t domain) {
  Input out;
  out.indices.resize(n);
  out.values.resize(n);
  uint64_t state = 0x3141592653589793ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.indices[i] = static_cast<uint32_t>((state >> 20) & (domain - 1u));
    state = state * 2862933555777941757ULL + 3037000493ULL;
    out.values[i] = static_cast<uint32_t>((state & 127u) + 1u);
  }
  return out;
}

uint64_t expected_hash(
    const std::vector<uint32_t>& indices,
    const std::vector<uint32_t>& values,
    uint32_t domain) {
  std::vector<uint64_t> accum(static_cast<std::size_t>(domain), 0);
  for (std::size_t i = 0; i < indices.size(); ++i) {
    accum[static_cast<std::size_t>(indices[i])] += static_cast<uint64_t>(values[i]);
  }
  uint64_t hash = 1469598103934665603ULL;
  for (uint64_t value : accum) {
    hash = mix(hash, value);
  }
  return hash;
}

}  // namespace

int main() {
  constexpr std::size_t kCount = 1u << 23;
  constexpr uint32_t kDomain = 1u << 16;
  constexpr int kIters = 12;

  const Input input = make_input(kCount, kDomain);
  const uint64_t expected = expected_hash(input.indices, input.values, kDomain);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual =
      sparse_accumulator_hash(input.indices, input.values, kDomain, kIters);
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
