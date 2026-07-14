public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    long total = 0;
    int len = left.length;
    for (int iter = 0; iter < iters; iter++) {
      long currentPopcount = 0;
      for (int i = 0; i < len; i++) {
        currentPopcount += Long.bitCount(left[i] & right[i]);
      }
      total = currentPopcount;
    }
    return total;
  }
}