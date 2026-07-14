public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    int n = deltas.length;
    long magic = 0x9E3779B1L;
    long constPart = n * magic;
    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      long sum = 0;
      for (int i = 0; i < n; i++) {
        current += deltas[i];
        sum += current;
      }
      total = sum + constPart;
    }
    return total;
  }
}