public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    int n = deltas.length;
    long c = 0x9E3779B1L;
    for (int iter = 0; iter < iters; iter++) {
      long cur = 0;
      for (int i = 0; i < n; i++) {
        cur += deltas[i];
        total += cur + c;
      }
    }
    return total;
  }
}