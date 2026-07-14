public class Solution {
  public static long run(int[] deltas, int iters) {
    final int n = deltas.length;
    final long constant = 0x9E3779B1L;
    final long offset = (long) n * constant;
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      long sum = 0;
      for (int i = 0; i < n; i++) {
        sum += (long) deltas[i] * (n - i);
      }
      total = sum + offset;
    }
    return total;
  }
}