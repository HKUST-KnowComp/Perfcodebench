public class Solution {
  private static final long OFFSET = 0x9E3779B1L;
  public static long run(int[] deltas, int iters) {
    long total = 0;
    final int len = deltas.length;
    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      long iterTotal = 0;
      for (int i = 0; i < len; i++) {
        current += deltas[i];
        iterTotal += current + OFFSET;
      }
      total = iterTotal;
    }
    return total;
  }
}