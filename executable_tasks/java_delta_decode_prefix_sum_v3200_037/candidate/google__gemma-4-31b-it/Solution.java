public class Solution {
  public static long run(int[] deltas, int iters) {
    int n = deltas.length;
    long weightedSum = 0;
    for (int i = 0; i < n; i++) {
      weightedSum += (long) deltas[i] * (n - i);
    }
    
    long constantSum = (long) n * 0x9E3779B1L;
    long totalPerIter = weightedSum + constantSum;
    
    return totalPerIter * iters;
  }
}