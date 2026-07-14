public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    long current = 0;
    for (int iter = 0; iter < iters; iter++) {
      long sum = 0;
      current = 0;
      for (int i = 0; i < deltas.length; i++) {
        current += deltas[i];
        sum += current + 0x9E3779B1L;
      }
      total += sum;
    }
    return total;
  }
}