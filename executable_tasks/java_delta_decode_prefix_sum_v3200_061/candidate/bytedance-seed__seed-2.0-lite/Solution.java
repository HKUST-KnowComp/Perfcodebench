public class Solution {
  public static long run(int[] deltas, int iters) {
    if (deltas.length == 0 || iters == 0) return 0;
    final long perIterConst = 0x9E3779B1L * deltas.length;
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      long sumPrefix = 0;
      for (int delta : deltas) {
        current += delta;
        sumPrefix += current;
      }
      total = sumPrefix + perIterConst;
    }
    return total;
  }
}