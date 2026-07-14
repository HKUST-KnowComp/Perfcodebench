public class Solution {
  public static long run(int[] deltas, int iters) {
    int n = deltas.length;
    long totalSumPerIter = 0;
    
    // The sum of prefix sums is: 
    // S = sum_{i=0 to n-1} (sum_{j=0 to i} deltas[j])
    // S = sum_{j=0 to n-1} (deltas[j] * (n - j))
    for (int i = 0; i < n; i++) {
      totalSumPerIter += (long) deltas[i] * (n - i);
    }
    
    // The baseline adds 0x9E3779B1L for every element in the values array
    long constantOffset = (long) n * 0x9E3779B1L;
    long totalPerIter = totalSumPerIter + constantOffset;
    
    // The baseline resets total = 0 inside the loop, but the return is the result of the last iteration
    // Wait, looking at the baseline: 
    // for (int iter = 0; iter < iters; iter++) {
    //   ... 
    //   total = 0;
    //   for (long v : values) total += v + 0x9E3779B1L;
    // }
    // return total;
    // This means only the result of the final iteration is returned.
    
    return totalPerIter;
  }
}