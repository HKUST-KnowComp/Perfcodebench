public class Solution {
  public static long run(int[] deltas, int iters) {
    if (iters == 0) return 0L;
    long total = 0L;
    long current = 0L;
    int len = deltas.length;
    for (int i = 0; i < len; i++) {
      current += deltas[i];
      total += current + 0x9E3779B1L;
    }
    return total;
  }
}