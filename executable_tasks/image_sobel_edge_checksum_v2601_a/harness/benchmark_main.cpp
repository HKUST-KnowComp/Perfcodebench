#include "interface.h"

#include <chrono>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>

namespace {

std::vector<uint8_t> make_input(int width, int height) {
  std::vector<uint8_t> image(static_cast<std::size_t>(width) * static_cast<std::size_t>(height));
  uint64_t state = 0x9e3779b97f4a7c15ULL;
  for (int y = 0; y < height; ++y) {
    for (int x = 0; x < width; ++x) {
      state = state * 6364136223846793005ULL + 1442695040888963407ULL;
      image[static_cast<std::size_t>(y) * static_cast<std::size_t>(width) + static_cast<std::size_t>(x)] =
          static_cast<uint8_t>((state >> 56) ^ (x * 13 + y * 7));
    }
  }
  return image;
}

uint8_t sobel_pixel(const uint8_t* prev, const uint8_t* cur, const uint8_t* next, int x) {
  const int gx =
      -static_cast<int>(prev[x - 1]) + static_cast<int>(prev[x + 1]) -
      2 * static_cast<int>(cur[x - 1]) + 2 * static_cast<int>(cur[x + 1]) -
      static_cast<int>(next[x - 1]) + static_cast<int>(next[x + 1]);
  const int gy =
      static_cast<int>(prev[x - 1]) + 2 * static_cast<int>(prev[x]) + static_cast<int>(prev[x + 1]) -
      static_cast<int>(next[x - 1]) - 2 * static_cast<int>(next[x]) - static_cast<int>(next[x + 1]);
  const int mag = std::abs(gx) + std::abs(gy);
  return static_cast<uint8_t>(mag > 255 ? 255 : mag);
}

uint64_t checksum_bytes(const std::vector<uint8_t>& bytes) {
  uint64_t hash = 1469598103934665603ULL;
  for (uint8_t b : bytes) {
    hash ^= static_cast<uint64_t>(b);
    hash *= 1099511628211ULL;
  }
  return hash;
}

uint64_t expected_checksum(const std::vector<uint8_t>& image, int width, int height) {
  std::vector<uint8_t> out(static_cast<std::size_t>(width - 2) * static_cast<std::size_t>(height - 2));
  for (int y = 1; y < height - 1; ++y) {
    const uint8_t* prev = image.data() + static_cast<std::size_t>(y - 1) * static_cast<std::size_t>(width);
    const uint8_t* cur = image.data() + static_cast<std::size_t>(y) * static_cast<std::size_t>(width);
    const uint8_t* next = image.data() + static_cast<std::size_t>(y + 1) * static_cast<std::size_t>(width);
    uint8_t* dst = out.data() + static_cast<std::size_t>(y - 1) * static_cast<std::size_t>(width - 2);
    for (int x = 1; x < width - 1; ++x) {
      dst[x - 1] = sobel_pixel(prev, cur, next, x);
    }
  }
  return checksum_bytes(out);
}

}  // namespace

int main() {
  constexpr int kWidth = 1536;
  constexpr int kHeight = 1024;
  constexpr int kIters = 4;
  const std::vector<uint8_t> image = make_input(kWidth, kHeight);
  const uint64_t expected = expected_checksum(image, kWidth, kHeight);

  const auto start = std::chrono::steady_clock::now();
  const uint64_t actual = sobel_edge_checksum(image, kWidth, kHeight, kIters);
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
