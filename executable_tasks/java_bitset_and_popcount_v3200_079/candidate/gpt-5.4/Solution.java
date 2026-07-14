public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    final int n = left.length;
    long total = 0L;

    for (int iter = 0; iter < iters; iter++) {
      long sum = 0L;
      for (int i = 0; i < n; i++) {
        sum += Long.bitCount(left[i] & right[i]);
      }
      total = sum;
    }

    return total;
  }
}
