#pragma once

#include <cstdint>
#include <string>

uint64_t newline_split_checksum(const std::string& text, int iters);

#include "interface.h"

#include <string>

namespace {

uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  uint64_t hash = 0;
  for (int iter = 0; iter < iters; ++iter) {
    hash = 1469598103934665603ULL;
    std::size_t start = 0;
    while (start < text.size()) {
      std::size_t end = text.find('\n', start);
      if (end == std::string::npos) {
        end = text.size();
      }
      uint64_t len = end - start;
      uint64_t first_char = (len > 0) ? static_cast<unsigned char>(text[start]) : 0;
      hash = mix(hash, len);
      hash = mix(hash, first_char);
      start = end + 1;
    }
  }
  return hash;
}