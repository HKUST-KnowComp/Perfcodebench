public class Solution {
  public static long run(int[] deltas, int iters) {
    long total = 0;
    long current = 0;
    int n = deltas.length;
    for (int i = 0; i < n; i++) {
      current += deltas[i];
      total += current;
    }
    total += (long) n * 0x9E3779B1L;
    return total;
  }
}