public class Solution {
  public static long run(int[] deltas, int iters) {
    final long constant = 0x9E3779B1L;
    final int n = deltas.length;
    final long perIterConstant = n * constant;
    long finalTotal = 0;
    
    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      long sumPrefix = 0;
      for (int i = 0; i < n; i++) {
        current += deltas[i];
        sumPrefix += current;
      }
      finalTotal = sumPrefix + perIterConstant;
    }
    return finalTotal;
  }
}