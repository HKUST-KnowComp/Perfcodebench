public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    long total = 0;
    int n = left.length;
    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      int i = 0;
      for (; i <= n - 4; i += 4) {
        current += Long.bitCount(left[i] & right[i])
                 + Long.bitCount(left[i+1] & right[i+1])
                 + Long.bitCount(left[i+2] & right[i+2])
                 + Long.bitCount(left[i+3] & right[i+3]);
      }
      for (; i < n; i++) {
        current += Long.bitCount(left[i] & right[i]);
      }
      total = current;
    }
    return total;
  }
}
