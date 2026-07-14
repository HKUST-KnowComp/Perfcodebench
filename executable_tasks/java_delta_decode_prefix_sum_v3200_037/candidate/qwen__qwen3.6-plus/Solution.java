public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    long n = deltas.length;
    long constantTerm = n * 0x9E3779B1L;
    for (int iter = 0; iter < iters; iter++) {
      long sum = 0;
      long weight = n;
      for (int i = 0; i < n; i++) {
        sum += (long) deltas[i] * weight;
        weight--;
      }
      total = sum + constantTerm;
    }
    return total;
  }
}