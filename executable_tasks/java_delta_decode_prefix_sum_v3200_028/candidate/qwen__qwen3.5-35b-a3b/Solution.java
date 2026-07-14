public class Solution {
  public static long run(int[] deltas, int iters) {
    int n = deltas.length;
    long current = 0;
    long sum = 0;
    for (int i = 0; i < n; i++) {
      current += deltas[i];
      sum += current + 0x9E3779B1L;
    }
    return sum * iters;
  }
}