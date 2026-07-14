public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    int n = deltas.length;
    long constant = n * 0x9E3779B1L;
    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      long sumVals = 0;
      for (int i = 0; i < n; i++) {
        current += deltas[i];
        sumVals += current;
      }
      total = sumVals + constant;
    }
    return total;
  }
}