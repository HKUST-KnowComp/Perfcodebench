public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    final int n = left.length;
    long total = 0L;

    for (int iter = 0; iter < iters; iter++) {
      long sum = 0L;
      int i = 0;

      // Unroll to reduce loop overhead and help the JIT optimize better.
      for (; i + 3 < n; i += 4) {
        sum += Long.bitCount(left[i] & right[i]);
        sum += Long.bitCount(left[i + 1] & right[i + 1]);
        sum += Long.bitCount(left[i + 2] & right[i + 2]);
        sum += Long.bitCount(left[i + 3] & right[i + 3]);
      }

      for (; i < n; i++) {
        sum += Long.bitCount(left[i] & right[i]);
      }

      total = sum;
    }

    return total;
  }
}
