public class Solution {
  public static long run(int[] deltas, int iters) {
    if (iters <= 0) return 0L;
    final int[] a = deltas;
    final int n = a.length;
    long current = 0L;
    long total = 0L;
    for (int i = 0; i < n; i++) {
      current += a[i];
      total += current;
    }
    total += 0x9E3779B1L * (long) n;
    return total;
  }
}