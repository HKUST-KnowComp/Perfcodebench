public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    final int len = deltas.length;
    final long offset = 0x9E3779B1L;
    
    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      long iterTotal = 0;
      for (int i = 0; i < len; i++) {
        current += deltas[i];
        iterTotal += (current + offset);
      }
      total = iterTotal;
    }
    return total;
  }
}