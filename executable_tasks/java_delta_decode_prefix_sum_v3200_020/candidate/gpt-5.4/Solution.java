public class Solution {
  public static long run(int[] deltas, int iters) {
    final int[] a = deltas;
    final int n = a.length;
    final long bias = 0x9E3779B1L;
    final long biasSum = bias * (long) n;

    long total = 0L;
    for (int iter = 0; iter < iters; iter++) {
      long current = 0L;
      long sum = 0L;
      for (int i = 0; i < n; i++) {
        current += a[i];
        sum += current;
      }
      total = sum + biasSum;
    }
    return total;
  }
}
