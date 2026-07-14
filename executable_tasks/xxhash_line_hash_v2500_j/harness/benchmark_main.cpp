#include "interface.h"

#include "xxhash.h"

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <random>
#include <string>

namespace {

uint64_t mix_hashes(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* p = input.data();
  const char* const end = p + input.size();
  while (p < end) {
    const char* line_end = p;
    while (line_end < end && *line_end != '\n') {
      ++line_end;
    }
    acc ^= static_cast<uint64_t>(XXH64(p, static_cast<std::size_t>(line_end - p), 0));
    acc *= 1099511628211ULL;
    p = (line_end < end) ? (line_end + 1) : line_end;
  }
  return acc;
}

std::string make_input(std::size_t lines) {
  static constexpr char kAlphabet[] =
      "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-";
  std::mt19937 rng(20260430);
  std::uniform_int_distribution<int> len_dist(8, 72);
  std::uniform_int_distribution<int> char_dist(0, static_cast<int>(sizeof(kAlphabet) - 2));
  std::string out;
  out.reserve(lines * 40);
  for (std::size_t i = 0; i < lines; ++i) {
    const int len = len_dist(rng);
    for (int j = 0; j < len; ++j) {
      out.push_back(kAlphabet[char_dist(rng)]);
    }
    out.push_back('\n');
  }
  return out;
}

}  // namespace

int main() {
    const std::string input = make_input(43800000);
  const uint64_t expected = mix_hashes(input);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = hash_lines(input);
  const auto end = std::chrono::steady_clock::now();

  const bool ok = actual == expected;
  const long long elapsed_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  std::cout << std::boolalpha;
  std::cout << "{"
            << "\"ok\":" << ok << ","
            << "\"elapsed_ns\":" << elapsed_ns << ","
            << "\"expected_hash\":" << expected << ","
            << "\"actual_hash\":" << actual << "}\n";
  return ok ? 0 : 1;
}
