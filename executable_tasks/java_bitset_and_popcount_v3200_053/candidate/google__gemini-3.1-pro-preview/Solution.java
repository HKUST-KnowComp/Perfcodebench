public class Solution {
  /**
   * Optimizes the bitset intersection popcount by using the JVM intrinsic Long.bitCount.
   * The baseline implementation was extremely inefficient, expanding bits into a byte array
   * and then summing them. This version performs the bitwise AND and popcount directly
   * on the long words, which typically maps to the hardware POPCNT instruction.
   */
  public static long run(long[] left, long[] right, int iters) {
    long total = 0;
    int len = left.length;
    
    // The benchmark harness measures the time taken for these iterations.
    for (int iter = 0; iter < iters; iter++) {
      total = 0;
      // Unrolling the loop manually can sometimes assist the JIT, 
      // though modern JVMs handle this well for simple loops.
      int i = 0;
      for (; i <= len - 4; i += 4) {
        total += Long.bitCount(left[i] & right[i]);
        total += Long.bitCount(left[i + 1] & right[i + 1]);
        total += Long.bitCount(left[i + 2] & right[i + 2]);
        total += Long.bitCount(left[i + 3] & right[i + 3]);
      }
      for (; i < len; i++) {
        total += Long.bitCount(left[i] & right[i]);
      }
    }
    
    return total;
  }
}