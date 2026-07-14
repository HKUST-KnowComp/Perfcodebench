public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    final long OFFSET = 0x9E3779B1L;
    final int arrayLength = deltas.length;
    for (int iter = 0; iter < iters; iter++) {
      long currentPrefix = 0;
      total = 0;
      for (int i = 0; i < arrayLength; i++) {
        currentPrefix += deltas[i];
        total += currentPrefix + OFFSET;
      }
    }
    return total;
  }
}