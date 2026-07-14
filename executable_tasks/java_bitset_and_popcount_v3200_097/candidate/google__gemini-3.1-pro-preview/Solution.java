public class Solution {
  /**
   * Computes the population count (number of set bits) of the bitwise AND of two long arrays.
   * 
   * @param left  The first bitset represented as a long array.
   * @param right The second bitset represented as a long array.
   * @param iters The number of times to repeat the calculation (benchmark harness).
   * @return The population count of the last iteration.
   */
  public static long run(long[] left, long[] right, int iters) {
    int n = left.length;
    long total = 0;

    // The benchmark harness repeats the operation 'iters' times.
    // We must ensure the result returned is the count from the final iteration.
    for (int iter = 0; iter < iters; iter++) {
      total = 0;
      // Iterate through the arrays and compute the popcount of the bitwise AND.
      // Long.bitCount is a highly optimized intrinsic (POPCNT on modern CPUs).
      for (int i = 0; i < n; i++) {
        total += Long.bitCount(left[i] & right[i]);
      }
    }

    return total;
  }
}