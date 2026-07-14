public class Solution {
  /**
   * Optimizes the delta decoding and prefix sum aggregation.
   * 
   * The baseline calculates:
   *   current_i = sum(deltas[0...i])
   *   total = sum_{i=0}^{N-1} (current_i + 0x9E3779B1L)
   * 
   * This can be simplified to:
   *   total = sum_{i=0}^{N-1} (deltas[i] * (N - i)) + N * 0x9E3779B1L
   * 
   * Since the result is overwritten in each iteration of 'iters' and the input 
   * 'deltas' is effectively constant, we can compute the result once in O(N) 
   * time and O(1) extra space.
   */
  public static long run(int[] deltas, int iters) {
    if (deltas == null || deltas.length == 0) {
      return 0;
    }

    final int n = deltas.length;
    final long constant = 0x9E3779B1L;
    
    // We only need to compute the result of the last iteration.
    // The sum of prefix sums: 
    // (d0) + (d0+d1) + (d0+d1+d2) ... 
    // = d0*N + d1*(N-1) + d2*(N-2) + ... + d_{N-1}*1
    
    long sumOfPrefixes = 0;
    long currentPrefixSum = 0;
    
    // Single pass to compute the sum of all prefix values
    for (int i = 0; i < n; i++) {
      currentPrefixSum += deltas[i];
      sumOfPrefixes += currentPrefixSum;
    }
    
    // Add the constant N times as per the original loop: total += v + constant
    return sumOfPrefixes + (n * constant);
  }
}