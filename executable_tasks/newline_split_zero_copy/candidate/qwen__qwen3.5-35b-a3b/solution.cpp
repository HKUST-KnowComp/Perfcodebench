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
    for (std::size_t i = 0; i < text.size(); ++i) {
      if (text[i] == '\n') {
        std::size_t len = i - start;
        uint64_t first = (len > 0) ? static_cast<unsigned char>(text[start]) : 0;
        hash = mix(hash, len);
        hash = mix(hash, first);
        start = i + 1;
      }
    }
    std::size_t len = text.size() - start;
    uint64_t first = (len > 0) ? static_cast<unsigned char>(text[start]) : 0;
    hash = mix(hash, len);
    hash = mix(hash, first);
  }
  return hash;
}