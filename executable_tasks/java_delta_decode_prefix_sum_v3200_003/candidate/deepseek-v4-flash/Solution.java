public class Solution {
  public static long run(int[] deltas, int iters) {
    if (iters == 0) return 0L;
    long current = 0L;
    long sum = 0L;
    for (int d : deltas) {
      current += d;
      sum += current;
    }
    return sum + (long) deltas.length * 0x9E3779B1L;
  }
}