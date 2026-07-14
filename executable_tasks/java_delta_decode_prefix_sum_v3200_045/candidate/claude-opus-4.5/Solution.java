public class Solution {
  public static long run(int[] deltas, int iters) {
    final int n = deltas.length;
    final long offset = 0x9E3779B1L;
    final long constantSum = (long) n * offset;
    long prefixSum = 0;
    {
      long weight = n;
      for (int i = 0; i < n; i++) {
        prefixSum += weight * deltas[i];
        weight--;
      }
    }
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      total = prefixSum + constantSum;
    }
    return total;
  }
}