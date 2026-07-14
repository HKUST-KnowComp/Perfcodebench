public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    long total = 0;
    int len = left.length;
    for (int iter = 0; iter < iters; iter++) {
      long currentIterTotal = 0;
      for (int i = 0; i < len; i++) {
        currentIterTotal += Long.bitCount(left[i] & right[i]);
      }
      total = currentIterTotal;
    }
    return total;
  }
}