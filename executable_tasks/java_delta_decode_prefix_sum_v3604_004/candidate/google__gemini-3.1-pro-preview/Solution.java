public class Solution {
  /**
   * Optimizes the delta decoding and prefix sum aggregation.
   * 
   * The original logic computes:
   * 1. values[i] = sum(deltas[0...i])
   * 2. total = sum(values[i] + 0x9E3779B1L) for i from 0 to n-1
   * 
   * This can be simplified mathematically:
   * total = sum(sum(deltas[0...i])) + n * 0x9E3779B1L
   * total = sum((n - i) * deltas[i]) + n * 0x9E3779B1L
   * 
   * By fusing these, we eliminate the O(N) long array allocation and the second pass over the data.
   */
  public static long run(int[] deltas, int iters) {
    if (deltas == null || deltas.length == 0) {
      return 0;
    }

    final int n = deltas.length;
    final long constantOffset = n * 0x9E3779B1L;
    long total = 0;

    // The outer loop is preserved to match the benchmark's expected workload,
    // though the result only depends on the final iteration.
    for (int iter = 0; iter < iters; iter++) {
      long weightedSum = 0;
      
      // Manual loop unrolling can be applied here, but the JIT usually handles 
      // simple linear summations well. We use a single pass to calculate 
      // the sum of prefix sums.
      for (int i = 0; i < n; i++) {
        // (n - i) is the number of times deltas[i] contributes to the total sum
        weightedSum += (long) deltas[i] * (n - i);
      }
      
      total = weightedSum + constantOffset;
    }

    return total;
  }
}