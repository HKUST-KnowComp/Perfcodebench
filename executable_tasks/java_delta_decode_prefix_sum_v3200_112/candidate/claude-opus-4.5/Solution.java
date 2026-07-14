public class Solution {
  public static long run(int[] deltas, int iters) {
    int n = deltas.length;
    long constantOffset = (long) n * 0x9E3779B1L;
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      long sum = 0;
      long multiplier = n;
      for (int i = 0; i < n; i++) {
        sum += multiplier * deltas[i];
        multiplier--;
      }
      total = sum + constantOffset;
    }
    return total;
  }
}
