public class Solution {
  public static long run(int[] deltas, int iters) {
    int n = deltas.length;
    long totalSumPerIter = 0;
    
    // The sum of prefix sums is sum_{i=0 to n-1} (n - i) * deltas[i]
    for (int i = 0; i < n; i++) {
      totalSumPerIter += (long) (n - i) * deltas[i];
    }
    
    // The constant 0x9E3779B1L is added n times per iteration
    long constantPart = (long) n * 0x9E3779B1L;
    long totalPerIter = totalSumPerIter + constantPart;
    
    // Multiply by number of iterations
    return totalPerIter * iters;
  }
}