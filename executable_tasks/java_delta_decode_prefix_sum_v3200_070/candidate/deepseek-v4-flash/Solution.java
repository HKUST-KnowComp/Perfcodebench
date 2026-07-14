public class Solution {
  public static long run(int[] deltas, int iters) {
    if (deltas == null || deltas.length == 0 || iters == 0) return 0L;
    long current = 0;
    long total = 0;
    for (int i = 0; i < deltas.length; i++) {
      current += deltas[i];
      total += current + 0x9E3779B1L;
    }
    return total;
  }
}