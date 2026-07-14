public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    int n = deltas.length;
    long constant = 0x9E3779B1L;
    for (int iter = 0; iter < iters; iter++) {
      long sum = 0;
      for (int i = 0; i < n; i++) {
        sum += (long) deltas[i] * (n - i);
      }
      sum += (long) n * constant;
      total = sum;
    }
    return total;
  }
}