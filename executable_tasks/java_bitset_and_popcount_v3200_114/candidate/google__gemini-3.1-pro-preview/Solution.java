public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    long total = 0;
    int n = left.length;
    for (int iter = 0; iter < iters; iter++) {
      long sum1 = 0, sum2 = 0, sum3 = 0, sum4 = 0;
      int i = 0;
      for (; i <= n - 4; i += 4) {
        sum1 += Long.bitCount(left[i] & right[i]);
        sum2 += Long.bitCount(left[i+1] & right[i+1]);
        sum3 += Long.bitCount(left[i+2] & right[i+2]);
        sum4 += Long.bitCount(left[i+3] & right[i+3]);
      }
      for (; i < n; i++) {
        sum1 += Long.bitCount(left[i] & right[i]);
      }
      total = sum1 + sum2 + sum3 + sum4;
    }
    return total;
  }
}
