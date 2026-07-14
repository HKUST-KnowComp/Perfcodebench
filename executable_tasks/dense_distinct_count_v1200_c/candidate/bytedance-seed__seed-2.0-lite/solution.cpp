#include "interface.h"

uint64_t distinct_count(const std::vector<uint32_t>& keys, uint32_t key_space, int iters) {
  std::vector<uint32_t> visited(key_space, 0);
  uint64_t final_count = 0;
  uint32_t generation = 1;
  for (int iter = 0; iter < iters; ++iter) {
    uint64_t count = 0;
    const uint32_t current_gen = generation;
    for (uint32_t key : keys) {
      if (visited[key] != current_gen) {
        count++;
        visited[key] = current_gen;
      }
    }
    final_count = count;
    generation++;
  }
  return final_count;
}