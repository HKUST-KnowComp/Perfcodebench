public class Solution {
  public static long run(int[] deltas, int iters) {
    int n = deltas.length;
    long perIterConstant = n * 0x9E3779B1L;
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      long iterSum = 0;
      for (int i = 0; i < n; i++) {
        current += deltas[i];
        iterSum += current;
      }
      total = iterSum + perIterConstant;
    }
    return total;
  }
}