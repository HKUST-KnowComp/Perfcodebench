uint64_t run(const char* const* rows, size_t row_count) {
  uint64_t hash = 1469598103934665603ULL;
  for (size_t i = 0; i < row_count; ++i) {
    int id, a, b, c;
    char flag[8];
    if (sscanf(rows[i], "r%d,%d,%d,%d,%7s", &id, &a, &b, &c, flag) != 5) {
      // Handle parsing error, but for simplicity, we assume input is always valid
      continue;
    }
    uint64_t v = (uint64_t)(b + c + 29);
    if (flag[0] == 'h' && flag[1] == 'o' && flag[2] == 't') v += 29ULL;
    hash ^= (v + 1ULL);
    hash *= 1099511628211ULL;
  }
  return hash;
}