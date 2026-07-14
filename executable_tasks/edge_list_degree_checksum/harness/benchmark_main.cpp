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
  std::vector<uint32_t> src;
  std::vector<uint32_t> dst;
};

Input make_input(std::size_t edges, uint32_t vertices) {
  Input out;
  out.src.resize(edges);
  out.dst.resize(edges);
  uint64_t state = 0x3141592653589793ULL;
  for (std::size_t i = 0; i < edges; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.src[i] = static_cast<uint32_t>((state >> 16) & (vertices - 1u));
    state = state * 2862933555777941757ULL + 3037000493ULL;
    out.dst[i] = static_cast<uint32_t>((state >> 20) & (vertices - 1u));
  }
  return out;
}

uint64_t expected_checksum(const std::vector<uint32_t>& src, uint32_t vertices) {
  std::vector<uint32_t> degrees(static_cast<std::size_t>(vertices), 0);
  for (uint32_t u : src) {
    ++degrees[static_cast<std::size_t>(u)];
  }
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t degree : degrees) {
    hash = mix(hash, degree);
  }
  return hash;
}

}  // namespace

int main() {
  constexpr std::size_t kEdges = 1u << 22;
  constexpr uint32_t kVertices = 1u << 16;
  constexpr int kIters = 12;

  const Input input = make_input(kEdges, kVertices);
  const uint64_t expected = expected_checksum(input.src, kVertices);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = degree_checksum(input.src, input.dst, kVertices, kIters);
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
