public class Solution {
  public static long run(int[] deltas, int iters) {
    final int n = deltas.length;
    final int[] a = deltas;
    final long perElemAdd = 0x9E3779B1L;
    final long addAll = perElemAdd * (long) n;

    long total = 0L;
    for (int iter = 0; iter < iters; iter++) {
      long cur = 0L;
      long sum = 0L;

      int i = 0;
      int unroll = n & ~3; // largest multiple of 4 <= n
      for (; i < unroll; i += 4) {
        cur += a[i];
        sum += cur;
        cur += a[i + 1];
        sum += cur;
        cur += a[i + 2];
        sum += cur;
        cur += a[i + 3];
        sum += cur;
      }
      for (; i < n; i++) {
        cur += a[i];
        sum += cur;
      }

      total = sum + addAll;
    }

    return total;
  }
}
