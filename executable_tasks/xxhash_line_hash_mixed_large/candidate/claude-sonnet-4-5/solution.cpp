#include "interface.h"

#include "xxhash.h"
#include <cstring>

uint64_t hash_lines(const std::string& input) {
  uint64_t acc = 1469598103934665603ULL;
  const char* p = input.data();
  const char* const end = p + input.size();
  const std::size_t total_size = input.size();
  
  // Use streaming API to reduce overhead
  XXH64_state_t state;
  
  // Process in batches to reduce accumulator update frequency
  constexpr int BATCH_SIZE = 8;
  uint64_t batch_hashes[BATCH_SIZE];
  
  while (p < end) {
    int batch_count = 0;
    
    // Collect up to BATCH_SIZE line hashes
    while (batch_count < BATCH_SIZE && p < end) {
      const char* line_start = p;
      const char* line_end;
      
      // Use memchr for faster newline search on larger remaining data
      std::size_t remaining = end - p;
      if (remaining > 64) {
        line_end = static_cast<const char*>(memchr(p, '\n', remaining));
        if (line_end == nullptr) {
          line_end = end;
        }
      } else {
        // Manual scan for small remaining data
        line_end = p;
        const char* scan_end = end - 3;
        while (line_end < scan_end) {
          if (line_end[0] == '\n') break;
          if (line_end[1] == '\n') { line_end += 1; break; }
          if (line_end[2] == '\n') { line_end += 2; break; }
          if (line_end[3] == '\n') { line_end += 3; break; }
          line_end += 4;
        }
        while (line_end < end && *line_end != '\n') {
          ++line_end;
        }
      }
      
      std::size_t line_len = line_end - line_start;
      batch_hashes[batch_count++] = XXH64(line_start, line_len, 0);
      
      p = (line_end < end) ? (line_end + 1) : line_end;
    }
    
    // Update accumulator with batch (unrolled)
    for (int i = 0; i < batch_count; ++i) {
      acc ^= batch_hashes[i];
      acc *= 1099511628211ULL;
    }
  }
  
  return acc;
}