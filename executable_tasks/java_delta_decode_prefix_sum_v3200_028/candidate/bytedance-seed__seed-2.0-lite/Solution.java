public class Solution {
  public static long run(int[] deltas, int iters) {
    final int n = deltas.length;
    final long addConstant = 0x9E3779B1L;
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      long currentPrefix = 0;
      long sumOfPrefixes = 0;
      for (int i = 0; i < n; i++) {
        currentPrefix += deltas[i];
        sumOfPrefixes += currentPrefix;
      }
      total = sumOfPrefixes + n * addConstant;
    }
    return total;
  }
}