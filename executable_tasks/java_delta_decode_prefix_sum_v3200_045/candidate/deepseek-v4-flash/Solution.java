public class Solution {
  public static long run(int[] deltas, int iters) {
    final int n = deltas.length;
    final long constAdd = (long) n * 0x9E3779B1L;
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      long sum = 0;
      for (int i = 0; i < n; i++) {
        current += deltas[i];
        sum += current;
      }
      total = sum + constAdd;
    }
    return total;
  }
}