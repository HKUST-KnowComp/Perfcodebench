public class Solution {
  public static long run(int[] deltas, int iters) {
    long cur = 0;
    long sum = 0;
    final long CONST = 0x9E3779B1L;
    for (int i = 0; i < deltas.length; i++) {
      cur += deltas[i];
      sum += cur;
    }
    return sum + (long) deltas.length * CONST;
  }
}