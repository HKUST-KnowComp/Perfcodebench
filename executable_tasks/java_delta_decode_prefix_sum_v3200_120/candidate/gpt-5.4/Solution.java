public class Solution {
  public static long run(int[] deltas, int iters) {
    final int[] a = deltas;
    final int n = a.length;
    long total = 0L;

    for (int iter = 0; iter < iters; iter++) {
      long current = 0L;
      long sum = 0L;
      int i = 0;

      // Unroll to reduce loop overhead and improve ILP while fusing decode + aggregation.
      for (; i + 7 < n; i += 8) {
        current += a[i];
        sum += current;
        current += a[i + 1];
        sum += current;
        current += a[i + 2];
        sum += current;
        current += a[i + 3];
        sum += current;
        current += a[i + 4];
        sum += current;
        current += a[i + 5];
        sum += current;
        current += a[i + 6];
        sum += current;
        current += a[i + 7];
        sum += current;
      }

      for (; i < n; i++) {
        current += a[i];
        sum += current;
      }

      total = sum + ((long) n * 0x9E3779B1L);
    }

    return total;
  }
}
