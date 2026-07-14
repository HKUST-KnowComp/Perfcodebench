public class Solution {
  public static long run(int[] deltas, int iters) {
    final int n = deltas.length;
    final long constant = 0x9E3779B1L;
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      long sum = 0;
      long prefixSum = 0;
      for (int i = 0; i < n; i++) {
        prefixSum += deltas[i];
        sum += prefixSum;
      }
      total = sum + n * constant;
    }
    return total;
  }
}
