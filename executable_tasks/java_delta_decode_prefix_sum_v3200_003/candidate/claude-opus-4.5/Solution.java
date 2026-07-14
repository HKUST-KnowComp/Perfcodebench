public class Solution {
  public static long run(int[] deltas, int iters) {
    final int n = deltas.length;
    final long constant = n * 0x9E3779B1L;
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      long sum = 0;
      long mult = n;
      int i = 0;
      final int limit = n - 3;
      for (; i < limit; i += 4) {
        sum += mult * deltas[i]
             + (mult - 1) * deltas[i + 1]
             + (mult - 2) * deltas[i + 2]
             + (mult - 3) * deltas[i + 3];
        mult -= 4;
      }
      for (; i < n; i++) {
        sum += mult * deltas[i];
        mult--;
      }
      total = sum + constant;
    }
    return total;
  }
}
