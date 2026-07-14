public class Solution {
  public static long run(int[] deltas, int iters) {
    long sumPrefix = 0;
    long current = 0;
    int n = deltas.length;
    for (int i = 0; i < n; i++) {
      current += deltas[i];
      sumPrefix += current;
    }
    long totalPerIter = sumPrefix + (long)n * 0x9E3779B1L;
    return totalPerIter * iters;
  }
}