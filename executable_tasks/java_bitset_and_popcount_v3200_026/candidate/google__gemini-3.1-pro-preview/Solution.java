public class Solution {
  /**
   * Computes the population count (number of set bits) of the bitwise AND of two long arrays.
   * 
   * @param left  The first bitset packed as a long array.
   * @param right The second bitset packed as a long array.
   * @param iters The number of iterations to perform the calculation.
   * @return The population count of the last iteration.
   */
  public static long run(long[] left, long[] right, int iters) {
    long total = 0;
    int len = left.length;
    
    // Perform the operation 'iters' times as required by the harness.
    for (int iter = 0; iter < iters; iter++) {
      total = 0;
      // Iterate through the arrays and compute the popcount of the bitwise AND.
      // Long.bitCount is a JVM intrinsic that maps to efficient CPU instructions.
      for (int i = 0; i < len; i++) {
        total += Long.bitCount(left[i] & right[i]);
      }
    }
    
    return total;
  }
}