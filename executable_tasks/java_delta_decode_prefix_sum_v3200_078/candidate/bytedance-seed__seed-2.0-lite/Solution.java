public class Solution {
  public static long run(int[] deltas, int iters) {
    final int n = deltas.length;
    if (n == 0 || iters == 0) return 0;
    final long constant = 0x9E3779B1L;
    final long totalConstantOffset = constant * n;
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      long sumOfPrefixes = 0;
      for (int i = 0; i < n; i++) {
        current += deltas[i];
        sumOfPrefixes += current;
      }
      total = sumOfPrefixes + totalConstantOffset;
    }
    return total;
  }
}