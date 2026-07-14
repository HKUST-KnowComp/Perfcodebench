public class Solution {
  public static long run(int[] deltas, int iters) {
    long sum = 0;
    int n = deltas.length;
    for (int i = 0; i < n; i++) {
      sum += (long) deltas[i] * (n - i);
    }
    sum += (long) n * 0x9E3779B1L;
    return sum * iters;
  }
}