public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    long total = 0;
    int n = left.length;
    for (int iter = 0; iter < iters; iter++) {
      long sum0 = 0, sum1 = 0, sum2 = 0, sum3 = 0;
      int i = 0;
      for (; i <= n - 4; i += 4) {
        sum0 += Long.bitCount(left[i] & right[i]);
        sum1 += Long.bitCount(left[i + 1] & right[i + 1]);
        sum2 += Long.bitCount(left[i + 2] & right[i + 2]);
        sum3 += Long.bitCount(left[i + 3] & right[i + 3]);
      }
      for (; i < n; i++) {
        sum0 += Long.bitCount(left[i] & right[i]);
      }
      total = sum0 + sum1 + sum2 + sum3;
    }
    return total;
  }
}
