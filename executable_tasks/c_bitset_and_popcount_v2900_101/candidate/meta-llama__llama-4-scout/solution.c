uint64_t run(const uint64_t* left, const uint64_t* right, size_t word_count, int iters) {
  uint64_t total = 0;
  for (int iter = 0; iter < iters; ++iter) {
    total = 0;
    for (size_t i = 0; i < word_count; ++i) {
      uint64_t word = left[i] & right[i];
      while (word) {
        word &= word - 1;
        total++;
      }
    }
  }
  return total;
}