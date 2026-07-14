public class Solution {
  public static long run(int[] deltas, int iters) {
    if (iters <= 0) return 0L;

    final int n = deltas.length;
    final int[] d = deltas;

    long acc = 0L;
    long w = n; // weight = (n - i)

    // Unrolled loop processes 4 elements per iteration
    int i = 0;
    int limit = n - (n & 3);
    for (; i < limit; i += 4) {
      acc += (long) d[i]     * w;
      acc += (long) d[i + 1] * (w - 1);
      acc += (long) d[i + 2] * (w - 2);
      acc += (long) d[i + 3] * (w - 3);
      w -= 4;
    }
    for (; i < n; i++) {
      acc += (long) d[i] * w;
      w--;
    }

    final long C = 0x9E3779B1L;
    // Each iteration yields the same result; return the last iteration's value
    return acc + C * (long) n;
  }
}
