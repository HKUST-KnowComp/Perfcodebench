public class Solution {
  private static final long C = 0x9E3779B1L;

  public static long run(int[] deltas, int iters) {
    // Baseline semantics return 0 when iters <= 0
    if (iters <= 0) return 0L;

    int n = deltas.length;
    long sumPrefixes = 0L;
    long current = 0L;

    int i = 0;
    int limit = n - 4;
    // Unrolled loop by 4 for reduced overhead
    for (; i <= limit; i += 4) {
      current += deltas[i];
      sumPrefixes += current;
      current += deltas[i + 1];
      sumPrefixes += current;
      current += deltas[i + 2];
      sumPrefixes += current;
      current += deltas[i + 3];
      sumPrefixes += current;
    }
    // Remainder
    for (; i < n; i++) {
      current += deltas[i];
      sumPrefixes += current;
    }

    // Add the per-element constant once
    return sumPrefixes + ((long) n) * C;
  }
}
