public class Solution {
  public static long run(int[] deltas, int iters) {
    if (iters <= 0) return 0L;
    long total = 0L;
    long current = 0L;
    for (int d : deltas) {
      current += d;
      total += current;
    }
    return total + (long)deltas.length * 0x9E3779B1L;
  }
}