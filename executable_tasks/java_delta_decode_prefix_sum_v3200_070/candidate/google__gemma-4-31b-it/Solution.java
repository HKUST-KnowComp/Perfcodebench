public class Solution {
  public static long run(int[] deltas, int iters) {
    int n = deltas.length;
    long totalSumOfPrefixes = 0;
    
    // The sum of prefix sums is sum_{i=0 to n-1} (sum_{j=0 to i} deltas[j])
    // This equals sum_{i=0 to n-1} (deltas[i] * (n - i))
    for (int i = 0; i < n; i++) {
      totalSumOfPrefixes += (long) deltas[i] * (n - i);
    }
    
    long constantOffsetSum = (long) n * 0x9E3779B1L;
    long singleIterTotal = totalSumOfPrefixes + constantOffsetSum;
    
    // The original code resets 'total' every iteration and returns the result of the last iteration.
    // However, the loop structure 'total = 0; for (long v : values) total += v + 0x9E3779B1L;' 
    // means 'total' is simply the sum of the last iteration's results.
    // If iters is 0, the original code returns 0. If iters > 0, it returns the result of one iteration.
    if (iters <= 0) return 0;
    return singleIterTotal;
  }
}