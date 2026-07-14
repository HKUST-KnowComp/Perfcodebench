public class Solution {
  public static long run(int[] deltas, int iters) {
    int n = deltas.length;
    long total = 0;
    long constantSum = (long) n * 0x9E3779B1L;
    
    // Precompute the sum of the prefix sums for one iteration
    // Sum_{i=0 to n-1} (Sum_{j=0 to i} deltas[j]) = Sum_{j=0 to n-1} (deltas[j] * (n - j))
    long prefixSumOfDeltas = 0;
    for (int i = 0; i < n; i++) {
      prefixSumOfDeltas += (long) deltas[i] * (n - i);
    }

    long iterationResult = prefixSumOfDeltas + constantSum;
    
    // The original code resets total = 0 inside the loop, 
    // so it effectively only returns the result of the last iteration.
    // However, the baseline says 'total = 0' inside the loop, meaning 
    // total is overwritten every time. 
    // Let's double check: 
    // for (int iter = 0; iter < iters; iter++) { ... total = 0; for (long v : values) total += v + ... }
    // This means the result is simply the result of the final iteration.
    
    return iterationResult;
  }
}