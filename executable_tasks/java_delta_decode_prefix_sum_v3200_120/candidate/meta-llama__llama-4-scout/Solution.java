public class Solution {
  public static long run(int[] deltas, int iters) {
    final long MAGIC = 0x9E3779B1L;
    long total = 0;
    long[] values = new long[deltas.length];
    long[] cumulative = new long[deltas.length];
    for (int iter = 0; iter < iters; iter++) {
      long cumulativeSum = 0;
      for (int i = 0; i < deltas.length; i++) {
        cumulativeSum += deltas[i];
        cumulative[i] = cumulativeSum;
      }
      for (int i = 0; i < deltas.length; i++) {
        total += cumulative[i] + MAGIC;
      }
    }
    return total;
  }
}