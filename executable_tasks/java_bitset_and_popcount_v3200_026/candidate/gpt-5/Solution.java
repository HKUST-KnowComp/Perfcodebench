public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    final long[] a = left;
    final long[] b = right;
    final int len = a.length;
    long total = 0L;
    for (int iter = 0; iter < iters; iter++) {
      int i = 0;
      final int limit = len & ~7; // round down to multiple of 8
      long s0 = 0L, s1 = 0L, s2 = 0L, s3 = 0L, s4 = 0L, s5 = 0L, s6 = 0L, s7 = 0L;
      while (i < limit) {
        s0 += Long.bitCount(a[i]     & b[i]);
        s1 += Long.bitCount(a[i + 1] & b[i + 1]);
        s2 += Long.bitCount(a[i + 2] & b[i + 2]);
        s3 += Long.bitCount(a[i + 3] & b[i + 3]);
        s4 += Long.bitCount(a[i + 4] & b[i + 4]);
        s5 += Long.bitCount(a[i + 5] & b[i + 5]);
        s6 += Long.bitCount(a[i + 6] & b[i + 6]);
        s7 += Long.bitCount(a[i + 7] & b[i + 7]);
        i += 8;
      }
      long sum = s0 + s1 + s2 + s3 + s4 + s5 + s6 + s7;
      while (i < len) {
        sum += Long.bitCount(a[i] & b[i]);
        i++;
      }
      total = sum;
    }
    return total;
  }
}