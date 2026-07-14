public class Solution {
  public static long run(int[] deltas, int iters) {
    final long constant = 0x9E3779B1L;
    final int len = deltas.length;
    final long constantSum = len * constant;
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      long prefixSum = 0;
      long current = 0;
      for (int i = 0; i < len; i++) {
        current += deltas[i];
        prefixSum += current;
      }
      total = prefixSum + constantSum;
    }
    return total;
  }
}