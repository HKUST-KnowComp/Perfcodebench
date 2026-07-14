public class Solution {
  public static long run(int[] deltas, int iters) {
    int n = deltas.length;
    long sumValues = 0;
    for (int i = 0; i < n; i++) {
      sumValues += (long)deltas[i] * (n - i);
    }
    long total = sumValues * iters + (long)n * iters * 0x9E3779B1L;
    return total;
  }
}