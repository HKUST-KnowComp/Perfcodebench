public class Solution {
  public static long run(int[] deltas, int iters) {
    final int n = deltas.length;
    if (n == 0) return 0L;
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      long sum = 0;
      for (int i = 0; i < n; i++) {
        sum += (long)(n - i) * deltas[i];
      }
      total = sum + (long)n * 0x9E3779B1L;
    }
    return total;
  }
}
