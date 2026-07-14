public class Solution {
  public static long run(int[] deltas, int iters) {
    long n = deltas.length;
    long sumPrefix = 0;
    for (int i = 0; i < n; i++) {
      sumPrefix += (long)(n - i) * deltas[i];
    }
    return sumPrefix + n * 0x9E3779B1L;
  }
}