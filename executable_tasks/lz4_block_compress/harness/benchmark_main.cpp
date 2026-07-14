#include "interface.h"

#include "lz4.h"

#include <chrono>
#include <iostream>
#include <random>
#include <string>
#include <vector>

namespace {

std::string make_input(std::size_t size) {
  std::mt19937 rng(12345);
  std::uniform_int_distribution<int> dist(0, 7);
  std::string s;
  s.resize(size);
  for (std::size_t i = 0; i < size; ++i) {
    s[i] = static_cast<char>('A' + dist(rng));
  }
  return s;
}

bool roundtrip_ok(const std::string& input, const std::string& compressed) {
  std::string restored;
  restored.resize(input.size());
  const int decoded = LZ4_decompress_safe(
      compressed.data(), restored.data(), static_cast<int>(compressed.size()), static_cast<int>(restored.size()));
  return decoded == static_cast<int>(input.size()) && restored == input;
}

}  // namespace

int main() {
  const std::string input = make_input(1 << 20);
  std::string compressed;

  const auto start = std::chrono::steady_clock::now();
  const int compressed_size = compress_payload(input, compressed);
  const auto end = std::chrono::steady_clock::now();

  const bool ok = compressed_size >= 0 && roundtrip_ok(input, compressed);
  const long long elapsed_ns =
      std::chrono::duration_cast<std::chrono::nanoseconds>(end - start).count();

  std::cout << std::boolalpha;
  std::cout << "{"
            << "\"ok\":" << ok << ","
            << "\"elapsed_ns\":" << elapsed_ns << ","
            << "\"compressed_size\":" << compressed_size << "}\n";
  return ok ? 0 : 1;
}
