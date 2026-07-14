public class Solution {
  public static long run(int[] deltas, int iters) {
    long[] values = new long[deltas.length];
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      for (int i = 0; i < deltas.length; i++) {
        current += deltas[i];
        values[i] = current;
      }
      total = 0;
      for (long v : values) total += v + 0x9E3779B1L;
    }
    return total;
  }
}
