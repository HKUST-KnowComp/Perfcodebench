public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    int n = deltas.length;
    long constantPart = (long) n * 0x9E3779B1L;
    for (int iter = 0; iter < iters; iter++) {
      long sumValues = 0;
      for (int i = 0; i < n; i++) {
        sumValues += (long) deltas[i] * (n - i);
      }
      total = sumValues + constantPart;
    }
    return total;
  }
}