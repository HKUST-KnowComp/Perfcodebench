public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    final long[] a = left;
    final long[] b = right;
    final int n = a.length;
    long total = 0L;
    for (int iter = 0; iter < iters; iter++) {
      long sum = 0L;
      int i = 0;
      int limit = n & ~3; // process in blocks of 4
      for (; i < limit; i += 4) {
        sum += Long.bitCount(a[i] & b[i]);
        sum += Long.bitCount(a[i + 1] & b[i + 1]);
        sum += Long.bitCount(a[i + 2] & b[i + 2]);
        sum += Long.bitCount(a[i + 3] & b[i + 3]);
      }
      for (; i < n; i++) {
        sum += Long.bitCount(a[i] & b[i]);
      }
      total = sum;
    }
    return total;
  }
}
