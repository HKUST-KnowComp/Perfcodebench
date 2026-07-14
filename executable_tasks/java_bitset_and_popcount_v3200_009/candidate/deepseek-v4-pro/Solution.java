public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      long sum = 0;
      int len = left.length;
      for (int i = 0; i < len; i++) {
        sum += Long.bitCount(left[i] & right[i]);
      }
      total = sum;
    }
    return total;
  }
}