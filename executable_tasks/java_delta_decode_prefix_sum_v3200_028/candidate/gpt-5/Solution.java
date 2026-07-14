public class Solution {
  public static long run(int[] deltas, int iters) {
    // Preserve baseline behavior for iters == 0
    if (iters <= 0) return 0L;

    final int n = deltas.length;
    final int[] arr = deltas;

    long current = 0L;
    long total = 0L;

    int i = 0;
    int limit = n & ~7; // round down to multiple of 8

    while (i < limit) {
      current += arr[i];
      total += current;
      current += arr[i + 1];
      total += current;
      current += arr[i + 2];
      total += current;
      current += arr[i + 3];
      total += current;
      current += arr[i + 4];
      total += current;
      current += arr[i + 5];
      total += current;
      current += arr[i + 6];
      total += current;
      current += arr[i + 7];
      total += current;
      i += 8;
    }

    for (; i < n; i++) {
      current += arr[i];
      total += current;
    }

    // Add the per-element constant once
    total += 0x9E3779B1L * (long) n;

    return total;
  }
}
