public class Solution {
  public static long run(int[] deltas, int iters) {
    final long offset = (long) deltas.length * 0x9E3779B1L;
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      long iterSum = 0;
      for (int d : deltas) {
        current += d;
        iterSum += current;
      }
      total = iterSum + offset;
    }
    return total;
  }
}