public class Solution {
  public static long run(int[] deltas, int iters) {
    if (iters <= 0) return 0L;
    long constant = 0x9E3779B1L;
    long current = 0L;
    long total = 0L;
    for (int delta : deltas) {
      current += delta;
      total += current + constant;
    }
    return total;
  }
}
