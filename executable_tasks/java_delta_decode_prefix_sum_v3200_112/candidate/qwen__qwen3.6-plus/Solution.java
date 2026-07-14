public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    long current = 0;
    final long C = 0x9E3779B1L;
    for (int d : deltas) {
      current += d;
      total += current + C;
    }
    return total;
  }
}