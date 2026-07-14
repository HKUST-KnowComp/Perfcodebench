public final class Solution {
  public static long run(long[] left, long[] right, int iters) {
    if (iters <= 0) return 0L;

    long total = 0L;
    int n = left.length;
    int i = 0;
    int limit = n & ~7; // process in chunks of 8

    while (i < limit) {
      total += Long.bitCount(left[i] & right[i])
             + Long.bitCount(left[i + 1] & right[i + 1])
             + Long.bitCount(left[i + 2] & right[i + 2])
             + Long.bitCount(left[i + 3] & right[i + 3])
             + Long.bitCount(left[i + 4] & right[i + 4])
             + Long.bitCount(left[i + 5] & right[i + 5])
             + Long.bitCount(left[i + 6] & right[i + 6])
             + Long.bitCount(left[i + 7] & right[i + 7]);
      i += 8;
    }
    for (; i < n; i++) {
      total += Long.bitCount(left[i] & right[i]);
    }

    return total;
  }
}
