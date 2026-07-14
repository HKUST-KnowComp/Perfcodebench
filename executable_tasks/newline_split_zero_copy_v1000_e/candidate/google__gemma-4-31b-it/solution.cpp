#include "interface.h"

#include <cstdint>
#include <string>

namespace {

inline uint64_t mix(uint64_t hash, uint64_t value) {
  hash ^= value;
  hash *= 1099511628211ULL;
  return hash;
}

}  // namespace

uint64_t newline_split_checksum(const std::string& text, int iters) {
  uint64_t final_hash = 0;
  const char* data = text.data();
  const size_t size = text.size();

  for (int iter = 0; iter < iters; ++iter) {
    uint64_t hash = 1469598103934665603ULL;
    size_t start = 0;
    
    while (start < size) {
      size_t len = 0;
      // Fast scan for newline
      while (start + len < size && data[start + len] != '\n') {
        len++;
      }
      
      // Process line length
      hash = mix(hash, (uint64_t)len);
      // Process first character
      hash = mix(hash, len == 0 ? 0u : static_cast<unsigned char>(data[start]));
      
      start += len + 1;
    }
    
    // Handle trailing newline case: if the text ends with '\n', 
    // the loop above finishes, but the reference logic (substr) 
    // would treat the empty space after the last '\n' as a line 
    // ONLY IF the loop condition was based on the index. 
    // However, the baseline `while (start < text.size())` 
    // does NOT process a final empty line if the string ends with '\n'.
    // Let's double check: if text = "a\n", start=0, end=1, lines.push("a"), start=2. 
    // 2 < 2 is false. Loop ends. Correct.
    
    final_hash = hash;
  }
  return final_hash;
}