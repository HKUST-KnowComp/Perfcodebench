public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    int n = deltas.length;
    long constant = 0x9E3779B1L;
    for (int iter = 0; iter < iters; iter++) {
      long iterTotal = 0;
      long weight = n;
      for (int i = 0; i < n; i++) {
        iterTotal += (long)deltas[i] * weight--;
      }
      iterTotal += n * constant;
      total = iterTotal;
    }
    return total;
  }
}