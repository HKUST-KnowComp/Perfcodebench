#include "interface.h"

#include <chrono>
#include <cstdint>
#include <iostream>
#include <string>
#include <vector>

namespace {

struct Input {
  std::vector<std::string> dictionary;
  std::vector<uint16_t> codes;
  std::vector<uint32_t> weights;
};

std::string make_token(uint32_t id) {
  std::string token(12, 'a');
  token[0] = static_cast<char>('a' + (id % 26u));
  token[1] = static_cast<char>('a' + ((id / 26u) % 26u));
  uint32_t x = id * 1103515245u + 12345u;
  for (int i = 2; i < 12; ++i) {
    x = x * 1664525u + 1013904223u;
    token[static_cast<std::size_t>(i)] = static_cast<char>('a' + ((x >> 27) % 26u));
  }
  return token;
}

Input make_input(std::size_t dict_size, std::size_t rows) {
  Input out;
  out.dictionary.reserve(dict_size);
  for (std::size_t i = 0; i < dict_size; ++i) {
    out.dictionary.push_back(make_token(static_cast<uint32_t>(i)));
  }
  out.codes.resize(rows);
  out.weights.resize(rows);
  uint64_t state = 0xfeedfacecafebeefULL;
  for (std::size_t i = 0; i < rows; ++i) {
    state = state * 6364136223846793005ULL + 1442695040888963407ULL;
    out.codes[i] = static_cast<uint16_t>(state & static_cast<uint64_t>(dict_size - 1));
    state = state * 2862933555777941757ULL + 3037000493ULL;
    out.weights[i] = static_cast<uint32_t>((state & 63u) + 1u);
  }
  return out;
}

bool matches_prefix(const std::string& s) {
  return s[0] == 'q' && s[1] <= 'm';
}

uint64_t expected_sum(
    const std::vector<std::string>& dictionary,
    const std::vector<uint16_t>& codes,
    const std::vector<uint32_t>& weights) {
  uint64_t sum = 0;
  for (std::size_t i = 0; i < codes.size(); ++i) {
    if (matches_prefix(dictionary[static_cast<std::size_t>(codes[i])])) {
      sum += static_cast<uint64_t>(weights[i]);
    }
  }
  return sum;
}

}  // namespace

int main() {
  constexpr std::size_t kDictionarySize = 4096;
  constexpr std::size_t kRows = 1u << 20;
  constexpr int kIters = 8;

  const Input input = make_input(kDictionarySize, kRows);
  const uint64_t expected = expected_sum(input.dictionary, input.codes, input.weights);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual =
      dictionary_prefix_weight_sum(input.dictionary, input.codes, input.weights, kIters);
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
