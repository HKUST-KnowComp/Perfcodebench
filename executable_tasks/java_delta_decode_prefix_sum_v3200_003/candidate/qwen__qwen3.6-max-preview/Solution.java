public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    int len = deltas.length;
    long constantAdd = (long)len * 0x9E3779B1L;
    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      long iterSum = 0;
      for (int i = 0; i < len; i++) {
        current += deltas[i];
        iterSum += current;
      }
      total = iterSum + constantAdd;
    }
    return total;
  }
}