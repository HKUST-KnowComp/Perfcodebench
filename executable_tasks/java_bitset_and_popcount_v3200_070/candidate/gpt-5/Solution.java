public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    final int n = left.length; // assume same length as baseline
    long total = 0L;

    for (int iter = 0; iter < iters; iter++) {
      long t = 0L;
      int i = 0;

      // Unroll by 8 for better ILP and fewer loop overheads
      int limit = n & ~7; // largest multiple of 8 <= n
      while (i < limit) {
        t += Long.bitCount(left[i] & right[i]);
        t += Long.bitCount(left[i + 1] & right[i + 1]);
        t += Long.bitCount(left[i + 2] & right[i + 2]);
        t += Long.bitCount(left[i + 3] & right[i + 3]);
        t += Long.bitCount(left[i + 4] & right[i + 4]);
        t += Long.bitCount(left[i + 5] & right[i + 5]);
        t += Long.bitCount(left[i + 6] & right[i + 6]);
        t += Long.bitCount(left[i + 7] & right[i + 7]);
        i += 8;
      }
      while (i < n) {
        t += Long.bitCount(left[i] & right[i]);
        i++;
      }

      total = t; // match baseline: result of the last iteration
    }

    return total;
  }
}
