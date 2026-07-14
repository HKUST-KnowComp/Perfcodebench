public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    long total = 0;
    long popcount = 0;
    for (int i = 0; i < left.length; i++) {
      popcount += Long.bitCount(left[i] & right[i]);
    }
    for (int iter = 0; iter < iters; iter++) {
      total = popcount;
    }
    return total;
  }
}