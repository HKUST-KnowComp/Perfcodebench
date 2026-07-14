public class Solution {
  public static long run(int[] deltas, int iters) {
    final int n = deltas.length;
    if (iters <= 0) return 0L;

    // Add the constant once per element at the end (mod 2^64 semantics preserved)
    final long addConstTotal = 0x9E3779B1L * (long) n;

    long total = 0L;
    for (int iter = 0; iter < iters; iter++) {
      long current = 0L;
      long sum = 0L;

      int i = 0;
      final int limit = n & ~7; // process in blocks of 8
      while (i < limit) {
        current += deltas[i];     sum += current;
        current += deltas[i + 1]; sum += current;
        current += deltas[i + 2]; sum += current;
        current += deltas[i + 3]; sum += current;
        current += deltas[i + 4]; sum += current;
        current += deltas[i + 5]; sum += current;
        current += deltas[i + 6]; sum += current;
        current += deltas[i + 7]; sum += current;
        i += 8;
      }
      for (; i < n; i++) {
        current += deltas[i];
        sum += current;
      }

      total = sum + addConstTotal;
    }
    return total;
  }
}
