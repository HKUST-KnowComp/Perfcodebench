public class Solution {
  public static long run(int[] deltas, int iters) {
    if (iters <= 0) return 0L;
    long total = 0;
    long current = 0;
    final long CONST = 0x9E3779B1L;
    for (int i = 0; i < deltas.length; i++) {
      current += deltas[i];
      total += current + CONST;
    }
    return total;
  }
}