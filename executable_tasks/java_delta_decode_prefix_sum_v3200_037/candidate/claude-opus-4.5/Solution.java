public class Solution {
  public static long run(int[] deltas, int iters) {
    final int n = deltas.length;
    final long constant = n * 0x9E3779B1L;
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      long sum = 0;
      int i = 0;
      final int limit = n - 3;
      for (; i < limit; i += 4) {
        long w0 = (long)(n - i)     * deltas[i];
        long w1 = (long)(n - i - 1) * deltas[i + 1];
        long w2 = (long)(n - i - 2) * deltas[i + 2];
        long w3 = (long)(n - i - 3) * deltas[i + 3];
        sum += w0 + w1 + w2 + w3;
      }
      for (; i < n; i++) {
        sum += (long)(n - i) * deltas[i];
      }
      total = sum + constant;
    }
    return total;
  }
}
