public class Solution {
  public static long run(int[] deltas, int iters) {
    if (iters <= 0) return 0;
    long current = 0;
    long sumPrefix = 0;
    int n = deltas.length;
    for (int i = 0; i < n; i++) {
      current += deltas[i];
      sumPrefix += current;
    }
    return sumPrefix + (long)n * 0x9E3779B1L;
  }
}