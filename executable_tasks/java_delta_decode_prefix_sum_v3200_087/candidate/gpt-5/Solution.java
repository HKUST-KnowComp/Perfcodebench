public class Solution {
  public static long run(int[] deltas, int iters) {
    // Preserve baseline behavior: if iters <= 0, return 0
    if (iters <= 0) return 0L;

    final int n = deltas.length;
    final int[] a = deltas;

    long current = 0L;
    long sum = 0L;

    // Unrolled loop by 4 for better throughput
    int i = 0;
    int limit = n & ~3; // largest multiple of 4 <= n
    for (; i < limit; i += 4) {
      long c0 = current + a[i];
      long s = sum + c0;

      long c1 = c0 + a[i + 1];
      s += c1;

      long c2 = c1 + a[i + 2];
      s += c2;

      long c3 = c2 + a[i + 3];
      s += c3;

      current = c3;
      sum = s;
    }
    for (; i < n; i++) {
      current += a[i];
      sum += current;
    }

    final long C = 0x9E3779B1L;
    long total = sum + ((long) n) * C;

    // Baseline overwrote total each iteration; since the value is the same each time,
    // returning this once-computed total preserves correctness.
    return total;
  }
}