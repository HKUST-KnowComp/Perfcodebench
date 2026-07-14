public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    int len = left.length;
    long total = 0L;
    for (int iter = 0; iter < iters; iter++) {
      long sum = 0L;
      int i = 0;
      int limit = len & ~7; // process in blocks of 8 for reduced loop overhead
      while (i < limit) {
        sum += Long.bitCount(left[i] & right[i])
             + Long.bitCount(left[i + 1] & right[i + 1])
             + Long.bitCount(left[i + 2] & right[i + 2])
             + Long.bitCount(left[i + 3] & right[i + 3])
             + Long.bitCount(left[i + 4] & right[i + 4])
             + Long.bitCount(left[i + 5] & right[i + 5])
             + Long.bitCount(left[i + 6] & right[i + 6])
             + Long.bitCount(left[i + 7] & right[i + 7]);
        i += 8;
      }
      for (; i < len; i++) {
        sum += Long.bitCount(left[i] & right[i]);
      }
      // Match baseline behavior: return the result from the last iteration
      total = sum;
    }
    return total;
  }
}
