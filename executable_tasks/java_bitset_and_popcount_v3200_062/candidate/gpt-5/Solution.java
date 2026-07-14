public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    long last = 0L;
    final long[] a = left;
    final long[] b = right;
    final int len = a.length; // assume same length as right, as in baseline

    for (int iter = 0; iter < iters; iter++) {
      long acc0 = 0L;
      long acc1 = 0L;

      int i = 0;
      int limit = len - 8;
      for (; i <= limit; i += 8) {
        acc0 += Long.bitCount(a[i]     & b[i]);
        acc0 += Long.bitCount(a[i + 1] & b[i + 1]);
        acc0 += Long.bitCount(a[i + 2] & b[i + 2]);
        acc0 += Long.bitCount(a[i + 3] & b[i + 3]);

        acc1 += Long.bitCount(a[i + 4] & b[i + 4]);
        acc1 += Long.bitCount(a[i + 5] & b[i + 5]);
        acc1 += Long.bitCount(a[i + 6] & b[i + 6]);
        acc1 += Long.bitCount(a[i + 7] & b[i + 7]);
      }

      for (; i < len; i++) {
        acc0 += Long.bitCount(a[i] & b[i]);
      }

      last = acc0 + acc1;
    }

    return last;
  }
}
