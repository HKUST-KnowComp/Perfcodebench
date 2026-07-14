public class Solution {
  public static long run(int[] deltas, int iters) {
    int n = deltas.length;
    long total = 0;
    long constantSum = (long) n * 0x9E3779B1L;
    
    // Precompute the sum of prefix sums for one iteration
    // Sum_{i=0 to n-1} PrefixSum_i = Sum_{i=0 to n-1} (n - i) * deltas[i]
    long singleIterSum = 0;
    for (int i = 0; i < n; i++) {
      singleIterSum += (long) (n - i) * deltas[i];
    }
    
    // The total is (singleIterSum + constantSum) repeated 'iters' times
    // We use a loop to match the baseline's potential for overflow behavior if iters is large,
    // though mathematically it is (singleIterSum + constantSum) * iters.
    for (int iter = 0; iter < iters; iter++) {
      total += singleIterSum + constantSum;
    }
    
    return total;
  }
}