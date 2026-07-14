public class Solution {
  /**
   * Computes the population count (number of set bits) of the bitwise AND of two long arrays.
   * 
   * The baseline implementation was inefficiently expanding bits into a byte array.
   * This version uses Long.bitCount, which is a HotSpot intrinsic that leverages 
   * the hardware POPCNT instruction for near-optimal performance.
   */
  public static long run(long[] left, long[] right, int iters) {
    long total = 0;
    int len = left.length;
    
    // The outer loop is preserved to match the baseline's structure for benchmarking.
    for (int iter = 0; iter < iters; iter++) {
      total = 0;
      // Iterate through the arrays and accumulate the bit count of the intersection.
      for (int i = 0; i < len; i++) {
        total += Long.bitCount(left[i] & right[i]);
      }
    }
    
    return total;
  }
}