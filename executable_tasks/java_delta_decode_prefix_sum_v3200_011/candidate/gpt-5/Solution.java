public class Solution {
  public static long run(int[] deltas, int iters) {
    if (iters <= 0) return 0L;

    final int[] a = deltas;
    final int n = a.length;
    long current = 0L;
    long sum = 0L;

    int i = 0;
    int limit = n & ~7; // process in chunks of 8
    while (i < limit) {
      current += a[i++]; sum += current;
      current += a[i++]; sum += current;
      current += a[i++]; sum += current;
      current += a[i++]; sum += current;
      current += a[i++]; sum += current;
      current += a[i++]; sum += current;
      current += a[i++]; sum += current;
      current += a[i++]; sum += current;
    }
    while (i < n) {
      current += a[i++];
      sum += current;
    }

    // Add the per-value constant once as C * n
    sum += 0x9E3779B1L * (long) n;

    return sum;
  }
}
