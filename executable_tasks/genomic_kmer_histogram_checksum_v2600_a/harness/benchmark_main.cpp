#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace {

std::string make_input(std::size_t n) {
  static constexpr char kAlphabet[] = {'A', 'C', 'G', 'T'};
  std::string out;
  out.resize(n);
  uint64_t state = 0x3141592653589793ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 2862933555777941757ULL + 3037000493ULL;
    out[i] = kAlphabet[(state >> 30) & 3u];
  }
  return out;
}

uint32_t code_of(char c) {
  switch (c) {
    case 'A': return 0u;
    case 'C': return 1u;
    case 'G': return 2u;
    default: return 3u;
  }
}

uint64_t expected_checksum(const std::string& dna, int k) {
  const std::size_t bins = 1u << (2 * k);
  std::vector<uint32_t> counts(bins, 0u);
  uint32_t code = 0u;
  const uint32_t mask = static_cast<uint32_t>(bins - 1u);
  for (int i = 0; i < k; ++i) {
    code = (code << 2) | code_of(dna[static_cast<std::size_t>(i)]);
  }
  counts[code] += 1u;
  for (std::size_t i = static_cast<std::size_t>(k); i < dna.size(); ++i) {
    code = ((code << 2) & mask) | code_of(dna[i]);
    counts[code] += 1u;
  }
  uint64_t hash = 1469598103934665603ULL;
  for (uint32_t count : counts) {
    hash ^= static_cast<uint64_t>(count);
    hash *= 1099511628211ULL;
  }
  return hash;
}

}  // namespace

int main() {
  constexpr std::size_t kLength = 200000;
  constexpr int kK = 5;
  constexpr int kIters = 18;
  const std::string dna = make_input(kLength);
  const uint64_t expected = expected_checksum(dna, kK);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = kmer_histogram_checksum(dna, kK, kIters);
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
