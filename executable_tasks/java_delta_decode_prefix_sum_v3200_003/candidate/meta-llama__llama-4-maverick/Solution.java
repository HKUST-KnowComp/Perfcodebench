public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      long sum = 0;
      for (int i = 0; i < deltas.length; i++) {
        current += deltas[i];
        sum += current;
      }
      total = sum + (long) deltas.length * 0x9E3779B1L;
    }
    return total;
  }
}