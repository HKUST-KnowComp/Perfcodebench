public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      total = 0;
      for (int d : deltas) {
        current += d;
        total += current + 0x9E3779B1L;
      }
    }
    return total;
  }
}
