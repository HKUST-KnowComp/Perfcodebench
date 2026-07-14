public class Solution {
  public static long run(int[] deltas, int iters) {
    final int N = deltas.length;
    final long C = 0x9E3779B1L;
    final long fixedOffset = C * N;
    long total = 0;
    for (int iter = 0; iter < iters; iter++) {
      long current = 0;
      long sumPrefixes = 0;
      for (int i = 0; i < N; i++) {
        current += deltas[i];
        sumPrefixes += current;
      }
      total = sumPrefixes + fixedOffset;
    }
    return total;
  }
}