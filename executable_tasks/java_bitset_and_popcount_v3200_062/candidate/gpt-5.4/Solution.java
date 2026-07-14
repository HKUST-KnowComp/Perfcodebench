public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    int len = left.length;
    long total = 0L;

    for (int iter = 0; iter < iters; iter++) {
      long sum = 0L;
      int i = 0;

      // Unroll to reduce loop overhead and improve throughput.
      for (; i + 7 < len; i += 8) {
        sum += Long.bitCount(left[i] & right[i]);
        sum += Long.bitCount(left[i + 1] & right[i + 1]);
        sum += Long.bitCount(left[i + 2] & right[i + 2]);
        sum += Long.bitCount(left[i + 3] & right[i + 3]);
        sum += Long.bitCount(left[i + 4] & right[i + 4]);
        sum += Long.bitCount(left[i + 5] & right[i + 5]);
        sum += Long.bitCount(left[i + 6] & right[i + 6]);
        sum += Long.bitCount(left[i + 7] & right[i + 7]);
      }

      for (; i < len; i++) {
        sum += Long.bitCount(left[i] & right[i]);
      }

      total = sum;
    }

    return total;
  }
}
