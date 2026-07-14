public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    // Compute popcount of (left & right) once; result does not depend on iters
    final int len = left.length; // baseline assumes equal lengths
    long sum = 0L;

    int i = 0;
    int limit = len - 8;
    // Unroll by 8 for better ILP and reduced loop overhead
    for (; i <= limit; i += 8) {
      long w0 = left[i] & right[i];
      long w1 = left[i + 1] & right[i + 1];
      long w2 = left[i + 2] & right[i + 2];
      long w3 = left[i + 3] & right[i + 3];
      long w4 = left[i + 4] & right[i + 4];
      long w5 = left[i + 5] & right[i + 5];
      long w6 = left[i + 6] & right[i + 6];
      long w7 = left[i + 7] & right[i + 7];
      sum += Long.bitCount(w0) + Long.bitCount(w1) + Long.bitCount(w2) + Long.bitCount(w3)
          + Long.bitCount(w4) + Long.bitCount(w5) + Long.bitCount(w6) + Long.bitCount(w7);
    }
    for (; i < len; i++) {
      sum += Long.bitCount(left[i] & right[i]);
    }

    return sum;
  }
}
