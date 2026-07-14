public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    long total = 0;
    int len = left.length;
    for (int iter = 0; iter < iters; iter++) {
      long currentPopCount = 0;
      for (int i = 0; i < len; i++) {
        currentPopCount += Long.bitCount(left[i] & right[i]);
      }
      total = currentPopCount;
    }
    return total;
  }
}