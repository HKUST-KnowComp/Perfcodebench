public class Solution {
  public static long run(int[] deltas, int iters) {
    int n = deltas.length;
    long base = 0;
    for (int i = 0; i < n; i++) {
      base += (long)(n - i) * deltas[i];
    }
    base += (long)n * 0x9E3779B1L;
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      total = base;
    }
    return total;
  }
}
