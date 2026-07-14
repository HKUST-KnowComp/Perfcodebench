#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

std::vector<uint64_t> make_words(std::size_t n) {
  std::vector<uint64_t> words(n);
  uint64_t state = 0x9e3779b97f4a7c15ULL;
  for (std::size_t i = 0; i < n; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    words[i] = state ^ (state >> 7);
  }
  return words;
}

uint64_t expected_count(const std::vector<uint64_t>& words) {
  uint64_t count = 0;
  for (uint64_t word : words) {
    count += static_cast<uint64_t>(__builtin_popcountll(word));
  }
  return count;
}

}  // namespace

int main() {
  constexpr std::size_t kWords = 1u << 20;
  constexpr int kIters = 16;

  const std::vector<uint64_t> words = make_words(kWords);
  const uint64_t expected = expected_count(words);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = bitmap_popcount(words, kIters);
  const auto end = std::chrono::steady_clock::now();

  const bool ok = actual == expected;
  const long long elapsed_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  std::cout << std::boolalpha;
  std::cout << "{"
            << "\"ok\":" << ok << ","
            << "\"elapsed_ns\":" << elapsed_ns << ","
            << "\"expected_count\":" << expected << ","
            << "\"actual_count\":" << actual << "}\n";
  return ok ? 0 : 1;
}
