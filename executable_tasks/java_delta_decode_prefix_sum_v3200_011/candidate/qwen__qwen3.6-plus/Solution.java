public class Solution {
  public static long run(int[] deltas, int iters) {
    if (iters <= 0) return 0;
    long total = 0;
    int n = deltas.length;
    long constant = 0x9E3779B1L;
    for (int i = 0; i < n; i++) {
      total += (long) deltas[i] * (n - i);
    }
    total += n * constant;
    return total;
  }
}