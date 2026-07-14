public class Solution {
  public static long run(int[] deltas, int iters) {
    int n = deltas.length;
    long total = 0;
    long constantPart = n * 0x9E3779B1L;
    for (int iter = 0; iter < iters; iter++) {
      long iterTotal = 0;
      for (int i = 0; i < n; i++) {
        iterTotal += (long) deltas[i] * (n - i);
      }
      total = iterTotal + constantPart;
    }
    return total;
  }
}