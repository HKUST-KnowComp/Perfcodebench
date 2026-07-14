public class Solution {
  public static long run(long[] left, long[] right, int iters) {
    long total = 0;
    int len = left.length;
    for (int iter = 0; iter < iters; iter++) {
      long currentIterSum = 0;
      for (int i = 0; i < len; i++) {
        currentIterSum += Long.bitCount(left[i] & right[i]);
      }
      total = currentIterSum;
    }
    // The original code resets total = 0 inside the loop and returns the sum of the last iteration.
    // To preserve the exact behavior of the baseline (which returns the popcount of the last iteration),
    // we calculate the sum for the final iteration.
    return total;
  }
}