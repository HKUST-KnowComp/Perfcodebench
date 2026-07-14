public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    int len = left.length;
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      total = 0;
      for (int i = 0; i < len; i++) {
        total += Long.bitCount(left[i] & right[i]);
      }
    }
    return total;
  }
}