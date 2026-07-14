public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    long[] prefixSum = new long[deltas.length];
    long current = 0;
    for (int i = 0; i < deltas.length; i++) {
      current += deltas[i];
      prefixSum[i] = current;
    }
    for (int iter = 0; iter < iters; iter++) {
      long tempTotal = 0;
      for (long v : prefixSum) {
        tempTotal += v + 0x9E3779B1L;
      }
      total += tempTotal;
    }
    return total;
  }
}