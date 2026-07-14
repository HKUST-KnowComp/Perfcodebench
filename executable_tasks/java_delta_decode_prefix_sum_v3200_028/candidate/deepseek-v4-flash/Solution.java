public class Solution {
  public static long run(int[] deltas, int iters) {
    int n = deltas.length;
    long sumPrefix = 0;
    long cum = 0;
    for (int i = 0; i < n; i++) {
      cum += deltas[i];
      sumPrefix += cum;
    }
    long baseTotal = sumPrefix + n * 0x9E3779B1L;
    return baseTotal * iters;
  }
}