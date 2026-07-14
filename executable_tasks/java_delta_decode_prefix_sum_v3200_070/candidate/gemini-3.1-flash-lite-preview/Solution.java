public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    int n = deltas.length;
    long magic = 0x9E3779B1L;
    
    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      long iterSum = 0;
      for (int i = 0; i < n; i++) {
        current += deltas[i];
        iterSum += (current + magic);
      }
      total = iterSum;
    }
    return total;
  }
}