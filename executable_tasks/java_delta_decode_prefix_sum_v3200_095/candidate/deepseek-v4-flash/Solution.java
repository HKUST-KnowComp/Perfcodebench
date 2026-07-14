public class Solution {
  private static final long CONST = 0x9E3779B1L;
  public static long run(int[] deltas, int iters) {
    if (iters == 0) return 0L;
    long current = 0L;
    long total = 0L;
    for (int i = 0; i < deltas.length; i++) {
      current += deltas[i];
      total += current + CONST;
    }
    return total;
  }
}