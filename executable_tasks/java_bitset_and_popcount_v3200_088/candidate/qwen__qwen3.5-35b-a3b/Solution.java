public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      long count = 0;
      for (int i = 0; i < left.length; i++) {
        count += Long.bitCount(left[i] & right[i]);
      }
      total += count;
    }
    return total;
  }
}