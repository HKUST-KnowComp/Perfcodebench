public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    final int n = deltas.length;
    final long addPerElement = 0x9E3779B1L;
    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      long iterTotal = 0;
      for (int i = 0; i < n; i++) {
        current += deltas[i];
        iterTotal += current + addPerElement;
      }
      total = iterTotal;
    }
    return total;
  }
}