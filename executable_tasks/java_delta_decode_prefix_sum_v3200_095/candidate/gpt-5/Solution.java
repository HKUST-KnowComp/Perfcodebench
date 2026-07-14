public class Solution {
  public static long run(int[] deltas, int iters) {
    if (iters <= 0) return 0L;

    final int n = deltas.length;
    long sum = 0L;

    // Compute sum of prefix sums using weighted deltas: sum += deltas[i] * (n - i)
    int i = 0;
    int w = n;
    int limit = n & ~7; // process in chunks of 8
    while (i < limit) {
      sum += (long) deltas[i]     * (long) (w);
      sum += (long) deltas[i + 1] * (long) (w - 1);
      sum += (long) deltas[i + 2] * (long) (w - 2);
      sum += (long) deltas[i + 3] * (long) (w - 3);
      sum += (long) deltas[i + 4] * (long) (w - 4);
      sum += (long) deltas[i + 5] * (long) (w - 5);
      sum += (long) deltas[i + 6] * (long) (w - 6);
      sum += (long) deltas[i + 7] * (long) (w - 7);
      i += 8;
      w -= 8;
    }
    while (i < n) {
      sum += (long) deltas[i++] * (long) (w--);
    }

    long c = 0x9E3779B1L;
    return sum + c * (long) n;
  }
}
