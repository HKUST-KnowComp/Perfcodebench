public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    int n = deltas.length;
    // Pre-calculate the constant to avoid repeated addition in the loop
    final long constant = 0x9E3779B1L;
    
    for (int iter = 0; iter < iters; iter++) {
      long currentPrefixSum = 0;
      long iterationTotal = 0;
      // Fuse delta decoding and summation into a single pass
      // This eliminates the need for the 'values' array, reducing memory traffic
      for (int i = 0; i < n; i++) {
        currentPrefixSum += deltas[i];
        iterationTotal += currentPrefixSum + constant;
      }
      total = iterationTotal;
    }
    return total;
  }
}