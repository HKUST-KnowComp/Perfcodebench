public class Solution {
  public static long run(int[] deltas, int iters) {
    if (deltas == null || deltas.length == 0 || iters == 0) {
      return 0L;
    }
    long running = 0L;
    long sum = 0L;
    for (int d : deltas) {
      running += d;
      sum += running;
    }
    return sum + (long) deltas.length * 0x9E3779B1L;
  }
}