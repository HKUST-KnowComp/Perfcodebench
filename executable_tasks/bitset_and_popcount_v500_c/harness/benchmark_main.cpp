#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

struct Input {
  std::vector<uint64_t> left;
  std::vector<uint64_t> right;
};

Input make_input(std::size_t words) {
  Input out;
  out.left.resize(words);
  out.right.resize(words);
  uint64_t state = 0x3141592653589793ULL;
  for (std::size_t i = 0; i < words; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.left[i] = state ^ (state >> 11);
    state = state * 2862933555777941757ULL + 3037000493ULL;
    out.right[i] = state ^ (state >> 9);
  }
  return out;
}

uint64_t expected_count(
    const std::vector<uint64_t>& left,
    const std::vector<uint64_t>& right) {
  uint64_t count = 0;
  for (std::size_t i = 0; i < left.size(); ++i) {
    count += static_cast<uint64_t>(__builtin_popcountll(left[i] & right[i]));
  }
  return count;
}

}  // namespace

int main() {
  constexpr std::size_t kWords = 1u << 20;
  constexpr int kIters = 16;

  const Input input = make_input(kWords);
  const uint64_t expected = expected_count(input.left, input.right);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = bitset_and_count(input.left, input.right, kIters);
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
