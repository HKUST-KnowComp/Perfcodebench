public class Solution {
  public static long run(int[] deltas, int iters) {
    final int[] a = deltas;
    final int n = a.length;
    final long bias = 0x9E3779B1L;
    long total = 0L;

    for (int iter = 0; iter < iters; iter++) {
      long current = 0L;
      long sum = 0L;
      int i = 0;

      int limit = n & ~7;
      while (i < limit) {
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
        i += 8;
      }

      while (i < n) {
        current += a[i++];
        sum += current;
      }

      total = sum + bias * n;
    }

    return total;
  }
}
