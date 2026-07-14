public class Solution {
  public static long run(int[] deltas, int iters) {
    if (iters <= 0) return 0L;
    int n = deltas.length;
    long sumPrefix = 0;
    for (int i = 0; i < n; i++) {
      sumPrefix += (long) deltas[i] * (n - i);
    }
    long total = sumPrefix + (long) n * 0x9E3779B1L;
    return total;
  }
}