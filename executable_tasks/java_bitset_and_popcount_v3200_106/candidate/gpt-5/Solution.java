public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    if (iters <= 0) return 0L;
    return andPopcount(left, right);
  }

  private static long andPopcount(long[] left, long[] right) {
    final int n = left.length;
    int i = 0;
    long s0 = 0L, s1 = 0L, s2 = 0L, s3 = 0L, s4 = 0L, s5 = 0L, s6 = 0L, s7 = 0L;

    final int limit = n & ~7; // process in blocks of 8
    while (i < limit) {
      s0 += Long.bitCount(left[i]     & right[i]);
      s1 += Long.bitCount(left[i + 1] & right[i + 1]);
      s2 += Long.bitCount(left[i + 2] & right[i + 2]);
      s3 += Long.bitCount(left[i + 3] & right[i + 3]);
      s4 += Long.bitCount(left[i + 4] & right[i + 4]);
      s5 += Long.bitCount(left[i + 5] & right[i + 5]);
      s6 += Long.bitCount(left[i + 6] & right[i + 6]);
      s7 += Long.bitCount(left[i + 7] & right[i + 7]);
      i += 8;
    }

    long sum = s0 + s1 + s2 + s3 + s4 + s5 + s6 + s7;
    for (; i < n; i++) {
      sum += Long.bitCount(left[i] & right[i]);
    }

    return sum;
  }
}
