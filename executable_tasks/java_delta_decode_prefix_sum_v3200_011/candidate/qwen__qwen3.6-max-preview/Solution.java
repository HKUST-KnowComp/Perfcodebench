public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    int n = deltas.length;
    long constantSum = n * 0x9E3779B1L;
    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      long sumValues = 0;
      for (int i = 0; i < n; i++) {
        current += deltas[i];
        sumValues += current;
      }
      total = sumValues + constantSum;
    }
    return total;
  }
}