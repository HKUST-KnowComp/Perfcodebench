#include "interface.h"

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
      size_t end = start;
      while (end < size && data[end] != '\n') {
        ++end;
      }
      
      size_t len = end - start;
      hash = mix(hash, (uint64_t)len);
      hash = mix(hash, len == 0 ? 0u : static_cast<unsigned char>(data[start]));
      
      start = end + 1;
    }
    
    // Handle trailing newline case: if the string ends with '\n', 
    // the loop above finishes, but the reference logic (substr) 
    // would treat the empty space after the last '\n' as a final empty line 
    // ONLY IF the loop condition was based on the index. 
    // Let's re-verify the baseline: 
    // while (start < text.size()) { ... start = end + 1; }
    // If text is "a\n", start=0, end=1, lines.push("a"), start=2. Loop ends.
    // If text is "a\nb", start=0, end=1, lines.push("a"), start=2, end=3, lines.push("b"), start=4. Loop ends.
    // The baseline logic does NOT add a trailing empty line if the string ends with '\n'.
    
    final_hash = hash;
  }
  return final_hash;
}