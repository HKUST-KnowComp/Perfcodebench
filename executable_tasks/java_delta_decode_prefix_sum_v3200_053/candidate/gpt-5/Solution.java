public class Solution {
  private static final long C = 0x9E3779B1L;

  public static long run(int[] deltas, int iters) {
    final int[] a = deltas;
    final int n = a.length;
    final int unrollMask = 7;
    final int limit = n - (n & unrollMask);
    final long cN = C * (long) n;

    long total = 0L;
    for (int iter = 0; iter < iters; iter++) {
      long cur = 0L;
      long t = 0L;
      int i = 0;

      while (i < limit) {
        cur += a[i++]; t += cur;
        cur += a[i++]; t += cur;
        cur += a[i++]; t += cur;
        cur += a[i++]; t += cur;
        cur += a[i++]; t += cur;
        cur += a[i++]; t += cur;
        cur += a[i++]; t += cur;
        cur += a[i++]; t += cur;
      }
      while (i < n) {
        cur += a[i++];
        t += cur;
      }

      total = t + cN;
    }
    return total;
  }
}
