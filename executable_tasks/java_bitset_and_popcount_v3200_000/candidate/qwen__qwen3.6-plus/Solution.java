public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    long total = 0;
    int n = left.length;
    for (int iter = 0; iter < iters; iter++) {
      long count = 0;
      for (int i = 0; i < n; i++) {
        count += Long.bitCount(left[i] & right[i]);
      }
      total = count;
    }
    return total;
  }
}