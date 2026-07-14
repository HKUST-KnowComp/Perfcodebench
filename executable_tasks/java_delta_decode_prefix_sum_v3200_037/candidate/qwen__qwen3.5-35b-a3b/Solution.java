public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    long current = 0;
    int len = deltas.length;
    
    for (int iter = 0; iter < iters; iter++) {
      current = 0;
      for (int i = 0; i < len; i++) {
        current += deltas[i];
        total += current + 0x9E3779B1L;
      }
    }
    return total;
  }
}