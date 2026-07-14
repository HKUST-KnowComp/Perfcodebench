public class Solution {
  public static long run(int[] deltas, int iters) {
    int n = deltas.length;
    long totalSumPerIter = 0;
    
    // The sum of prefix sums is: 
    // S = d[0] + (d[0]+d[1]) + (d[0]+d[1]+d[2]) ... 
    // S = sum_{i=0 to n-1} (d[i] * (n - i))
    for (int i = 0; i < n; i++) {
      totalSumPerIter += (long) deltas[i] * (n - i);
    }
    
    // Add the constant 0x9E3779B1L for each element in the array
    totalSumPerIter += (long) n * 0x9E3779B1L;
    
    // Multiply by the number of iterations
    return totalSumPerIter * iters;
  }
}