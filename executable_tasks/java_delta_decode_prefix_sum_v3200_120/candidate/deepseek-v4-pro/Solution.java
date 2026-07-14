public class Solution {
  public static long run(int[] deltas, int iters) {
    if (iters == 0) return 0L;
    long current = 0;
    long total = 0;
    for (int delta : deltas) {
      current += delta;
      total += current + 0x9E3779B1L;
    }
    return total;
  }
}