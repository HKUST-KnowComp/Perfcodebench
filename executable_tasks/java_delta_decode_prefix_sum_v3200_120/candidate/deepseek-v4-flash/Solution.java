public class Solution {
  public static long run(int[] deltas, int iters) {
    if (iters == 0) return 0;
    long current = 0;
    long sum = 0;
    for (int i = 0; i < deltas.length; i++) {
      current += deltas[i];
      sum += current;
    }
    return sum + (long) deltas.length * 0x9E3779B1L;
  }
}