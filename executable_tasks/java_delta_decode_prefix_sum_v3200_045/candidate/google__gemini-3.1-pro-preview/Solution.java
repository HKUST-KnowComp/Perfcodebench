public class Solution {
  public static long run(int[] deltas, int iters) {
    if (deltas == null || deltas.length == 0) {
      return 0;
    }

    final int n = deltas.length;
    final long k = 0x9E3779B1L;
    final long constantOffset = n * k;
    long total = 0;

    for (int iter = 0; iter < iters; iter++) {
      long sum = 0;
      int i = 0;
      
      // Unroll the loop to process 8 elements at a time for better ILP
      int limit = n - (n % 8);
      for (; i < limit; i += 8) {
        sum += (long) deltas[i]     * (n - i);
        sum += (long) deltas[i + 1] * (n - (i + 1));
        sum += (long) deltas[i + 2] * (n - (i + 2));
        sum += (long) deltas[i + 3] * (n - (i + 3));
        sum += (long) deltas[i + 4] * (n - (i + 4));
        sum += (long) deltas[i + 5] * (n - (i + 5));
        sum += (long) deltas[i + 6] * (n - (i + 6));
        sum += (long) deltas[i + 7] * (n - (i + 7));
      }

      // Process remaining elements
      for (; i < n; i++) {
        sum += (long) deltas[i] * (n - i);
      }

      total = sum + constantOffset;
    }

    return total;
  }
}