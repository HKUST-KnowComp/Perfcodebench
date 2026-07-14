public class Solution {
  public static long run(int[] deltas, int iters) {
    if (iters <= 0) return 0L;

    final int n = deltas.length;
    long current = 0L;
    long sum = 0L;

    int i = 0;
    int limit = n & ~7;
    while (i < limit) {
      current += deltas[i++]; sum += current;
      current += deltas[i++]; sum += current;
      current += deltas[i++]; sum += current;
      current += deltas[i++]; sum += current;
      current += deltas[i++]; sum += current;
      current += deltas[i++]; sum += current;
      current += deltas[i++]; sum += current;
      current += deltas[i++]; sum += current;
    }
    for (; i < n; i++) {
      current += deltas[i];
      sum += current;
    }

    // Add the per-element constant exactly as baseline, once per element
    sum += 0x9E3779B1L * (long) n;

    return sum;
  }
}
