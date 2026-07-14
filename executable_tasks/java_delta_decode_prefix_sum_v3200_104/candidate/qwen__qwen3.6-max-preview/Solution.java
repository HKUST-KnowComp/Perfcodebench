public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    int n = deltas.length;
    long constantPart = 0x9E3779B1L * n;
    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      long iterTotal = 0;
      for (int i = 0; i < n; i++) {
        current += deltas[i];
        iterTotal += current;
      }
      total = iterTotal + constantPart;
    }
    return total;
  }
}