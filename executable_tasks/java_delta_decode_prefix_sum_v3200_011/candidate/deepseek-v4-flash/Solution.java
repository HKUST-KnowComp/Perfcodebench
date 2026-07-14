public class Solution {
  public static long run(int[] deltas, int iters) {
    if (iters <= 0) return 0L;
    long current = 0L;
    long sum = 0L;
    int n = deltas.length;
    for (int i = 0; i < n; i++) {
      current += deltas[i];
      sum += current;
    }
    return sum + (long) n * 0x9E3779B1L;
  }
}