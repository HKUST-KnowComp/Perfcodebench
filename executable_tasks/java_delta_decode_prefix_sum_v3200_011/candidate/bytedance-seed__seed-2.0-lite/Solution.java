public class Solution {
  public static long run(int[] deltas, int iters) {
    final long CONSTANT = 0x9E3779B1L;
    long totalPrefixSum = 0;
    final int n = deltas.length;
    
    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      for (int i = 0; i < n; i++) {
        current += deltas[i];
        totalPrefixSum += current;
      }
    }
    
    return totalPrefixSum + (long) iters * n * CONSTANT;
  }
}