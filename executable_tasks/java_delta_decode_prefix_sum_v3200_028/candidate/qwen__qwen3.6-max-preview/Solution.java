public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    int n = deltas.length;
    long bias = n * 0x9E3779B1L;
    int[] d = deltas;
    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      long iterTotal = 0;
      for (int i = 0; i < n; i++) {
        current += d[i];
        iterTotal += current;
      }
      total = iterTotal + bias;
    }
    return total;
  }
}